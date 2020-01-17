#include "Pipeline.hpp"
#include <iterator>

namespace fpp {

    Pipeline::Pipeline() {
        setName("Pipeline");
    }

    Pipeline::~Pipeline() {
        try_throw(stop());
    }

    Code Pipeline::addElement(SharedProcessor processor, SourceType priority) { //TODO каша из кода
        switch (processor->type()) {
        case ProcessorType::Input:
            try_to(processor->init());
            try_to(processor->open());
            if (priority == SourceType::Mandatory) {
                _data_sources.push_back(processor);
            } else {
                _data_backup_sources.push_back(processor);
            }
            return Code::OK;
        case ProcessorType::Output:
            _data_sinks.push_back(processor);
            try_to(processor->init());
            try_to(determineSequence(processor));
            return Code::OK;
        case ProcessorType::Process:
        case ProcessorType::Unknown:
            return Code::INVALID_INPUT;
        }
        return Code::ERR;
    }

    void Pipeline::remElement(SharedProcessor processor) {
        _data_sinks.remove_if([processor](const auto& sink) {
            return sink->uid() == processor->uid();
        });
        _data_sources.remove_if([processor](const auto& source) {
            return source->uid() == processor->uid();
        });
    }

    void Pipeline::remElement(int64_t uid) {
        _data_sinks.remove_if([uid](const auto& sink) { return sink->uid() == uid; });
        _data_sources.remove_if([uid](const auto& source) { return source->uid() == uid; });

        findRoute(uid)->destroy();
        auto cond = [uid](const Route& route){ return route.contains(uid); };
        _route_list.erase(std::remove_if(_route_list.begin(), _route_list.end(), cond), _route_list.end());
    }

    void Pipeline::dump() const {
        log_info(toString());
    }

    Code Pipeline::init() { //TODO если инициализации нет, убрать реализацию метода 15.01
        log_info("Initialization started...");
        setInited(true);
        log_info("Processing started...");
        return Code::OK;
    }

    Code Pipeline::run() { //TODO
        bool all_processor_stopped = true;
        Code exit_code = Code::OK;
        _data_sources.for_each([&all_processor_stopped,&exit_code](const auto& source) {
            return_if(utils::error_code(source->exitCode())
                      , void()/*thread_processor->exitCode()*/);
            if (utils::exit_code(source->exitCode())) {
                exit_code = source->exitCode();
            }
            if (source->running()) {
                all_processor_stopped = false;
                return;
            } else {
                /* Выброс отработавшего процессора из пула */
            }
        });
        _data_sinks.for_each([&all_processor_stopped,&exit_code](const auto& sink) {
            return_if(utils::error_code(sink->exitCode())
                      , void()/*thread_processor->exitCode()*/);
            if (utils::exit_code(sink->exitCode())) {
                exit_code = sink->exitCode();
            }
            if (sink->running()) {
                all_processor_stopped = false;
                return;
            } else {
                /* Выброс отработавшего процессора из пула */
            }
        });
        return_if(all_processor_stopped, Code::END_OF_FILE);
        utils::sleep_for(LONG_DELAY_MS/* * 10*/);
        return Code::OK;
    }

    Code Pipeline::onStart() { // TODO
        return Code::OK;
    }

    Code Pipeline::onStop() {
        log_info("Stopping...");
        _data_sources.for_each([](auto& source){
            try_throw_static(source->stop());
        });
        log_info("Processing finished");
        return Code::OK;
    }

    Code Pipeline::createSequence(Route& route) {
        auto input_stream = findStream(route.inputStreamUid());
        auto output_stream = findStream(route.outputStreamUid());
        return_error_if(not_inited_ptr(input_stream)
                        , "Failed to find input stream"
                        , Code::ERR);
        return_error_if(not_inited_ptr(output_stream)
                        , "Failed to find output stream"
                        , Code::ERR);

        const IOParams params { input_stream->params, output_stream->params };

        try_to(params.out->completeFrom(params.in));

        input_stream->setUsed(true);
        output_stream->setUsed(true);

        try_to(route.append(findProcessor(params.in->contextUid())));

        bool rescaling_required     = utils::rescaling_required   (params);
        bool resampling_required    = utils::resampling_required  (params);
        bool video_filter_required  = utils::video_filter_required(params);
        bool audio_filter_required  = utils::audio_filter_required(params);
        bool transcoding_required   = utils::transcoding_required (params);

        auto out_context = dynamic_cast<MediaSink*>(findProcessor(output_stream->params->contextUid()).get()); //TODO кривой код: вынести IOPreset в Processor ?
        if (inited_ptr(out_context)) {
            video_filter_required = video_filter_required
                                    || out_context->outputFormatContext()->presetIs(IOPreset::Timelapse); //TODO кривой код
        }

        transcoding_required = (transcoding_required
                                || rescaling_required
                                || resampling_required);
        bool decoding_required = transcoding_required
                                || rescaling_required
                                || resampling_required
                                || video_filter_required
                                || audio_filter_required;
        bool encoding_required = transcoding_required
                                || rescaling_required
                                || resampling_required
                                || video_filter_required
                                || audio_filter_required;

        if (decoding_required) {
            try_to(route.append(std::make_shared<Decoder>(params)));
        }

        if (rescaling_required) {
            try_to(route.append(std::make_shared<Rescaler>(params)));
        }

        if (video_filter_required) { //TODO не универсальный код 15.01
            const int X = 4; //TODO "магическое" число
            std::string filters_descr = Filter::keep_every_frame(X) + Filter::Separator + Filter::set_pts(1.f / X);
            try_to(route.append(std::make_shared<VideoFilter>(params, filters_descr)));
        }

        if (audio_filter_required) {
            return Code::NOT_IMPLEMENTED;
        }

        if (resampling_required) {
            try_to(route.append(std::make_shared<Resampler>(params)));
        }

        if (encoding_required) {
            try_to(route.append(std::make_shared<Encoder>(params)));
        }

        try_to(route.append(findProcessor(params.out->contextUid())));

        _route_list.push_back(route);
//        try_to(simplifyRoutes()); //TODO вернуть вызов метода
        try_to(route.init());

        log_debug("Created route: " << route);
        log_error("input_stream  : " << input_stream->toString());
        log_error("output_stream : " << output_stream->toString());
        output_stream->init();
        return Code::OK;
    }

    SharedProcessor Pipeline::findProcessor(int64_t uid) {
        SharedProcessor result;
        auto finder = [uid,&result](const auto& source) {
            if (source->uid() == uid) {
                result = source;
            }
        };
        _data_sources.for_each(finder);
        _data_backup_sources.for_each(finder);
        _data_sinks.for_each(finder);
        return result;
    }

    Code Pipeline::simplifyRoutes() {
        return_if(_route_list.empty(), Code::OK);
        for (size_t i = 0; i < (_route_list.size() - 1); ++i) {
            for (size_t j = i + 1; j < _route_list.size(); ++j) {
                Route& route_one = _route_list[i];
                Route& route_two = _route_list[j];

                log_warning("");
                log_warning("Comparing ");
                log_warning(i << "] " << route_one);
                log_warning(j << "] " << route_two);

                auto sequence_one = route_one.processorSequence();
                auto sequence_two = route_two.processorSequence();

                size_t min_size = std::min(sequence_one.size(), sequence_two.size());

                for (size_t k = 0; k < min_size; ++k) {
                    if (k > 0) {
                        if (sequence_one[k]->uid() == sequence_two[k]->uid()) {
                            //уже один и тот же
                            break;
                        }
                    }
                    if_not(sequence_one[k]->equalTo(sequence_two[k])) {
                        log_warning("FORK POINT is " << sequence_one[k]->name());
                        //TODO склеить последовательности от 0 до k-1
                        if (k > 0) {
//                            ProcessorVector mutual(route_one.processorSequence().begin()
//                                                   , route_one.processorSequence().begin() + int64_t(k - 1));
                            ProcessorVector mutual;
                            for (size_t j = 0; j < k; ++j) {
                                mutual.push_back(route_one.processorSequence()[j]);
                            }
                            try_to(route_two.changePartTo(mutual));
                        }
                        break;
                    }
                }
            }
        }
        for (const auto& route : _route_list) {
            log_warning("Result: " << route);
        }
        return Code::OK;
    }

    SharedStream Pipeline::findStream(UID uid) {
        SharedStream ret_stream;
        auto finder = [uid,&ret_stream](const auto& proc) {
            auto proc_streams = proc->streams();
            auto ret_it = std::find_if(std::begin(proc_streams), std::end(proc_streams)
                         , [uid](const auto& stream) {
                return stream->params->streamUid() == uid;
            });
            if (ret_it != std::end(proc_streams)) { ret_stream = *ret_it; }
        };
        _data_sources.for_each(finder);
        _data_backup_sources.for_each(finder);
        _data_sinks.for_each(finder);
        return ret_stream;
    }

    Code Pipeline::determineSequence(const SharedProcessor output_processor) { //TODO говнокод
        const auto& output_streams = output_processor->streams();
        if (output_streams.empty()) {
            log_error(output_processor->name() << " doesn't has any stream.");
            return Code::NOT_INITED;
        }
        std::vector<Route> route_list;
        for (auto out_stream : output_streams) {
            auto best_in_stream = findBestInputStream(out_stream->type());
            return_error_if(not_inited_ptr(best_in_stream)
                            , "Failed to find input stream type " << out_stream->type()
                            , Code::INVALID_CALL_ORDER);
            Route route;
            try_to(route.setMetaRoute(best_in_stream->params->streamUid()
                                      , out_stream->params->streamUid()));
            try_to(createSequence(route));
            route_list.push_back(route);
        }
        for (auto& route : route_list) { //Нельзя старттовать процессоры в папйлане, пока все потоки не образуют маршруты
            route.startAll();
        }
        return Code::OK;
    }

    std::string Pipeline::toString() const { //TODO восстановить метод
        std::string dump_str;

        /* Вывод информации о контекстах
         * |  Input #0, rtsp, from 'rtsp://admin:admin@192.168.10.3':
         * |    Stream #0:0: Video: h264, 1920x1080 [16:9], 23 fps, 400 kb/s
         * |    Stream #0:1: Audio: pcm_mulaw, 16000 Hz, 1 channels, s16, 128 kb/s
         * |  Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/ytub-8t5w-asjj-avyf':
         * |    Stream #0:0: Video: h264, 1920x1080, 400 kb/s
         * |    Stream #0:1: Audio: aac, 44100 Hz, stereo, 128 kb/s
        */
//        for (auto&& source : mediaSources()) {
//            dump_str += "\n";
//            dump_str += TAB;
//            dump_str += source->toString();
//            for (auto i = 0; i < source->inputFormatContext().numberStream(); i++) {
//                dump_str += ":\n";
//                dump_str += TAB;
//                dump_str += TAB;
//                dump_str += source->inputFormatContext().stream(i)->toString(); //TODO tut
//            }
//        }

//        for (auto&& sink : mediaSinks()) {
//            dump_str += "\n";
//            dump_str += TAB;
//            dump_str += sink->toString();
//            for (auto i = 0; i < sink->outputFormatContext().numberStream(); i++) {
//                dump_str += ":\n";
//                dump_str += TAB;
//                dump_str += TAB;
//                dump_str += sink->outputFormatContext().toString(); //TODO tut
//            }
//        }

        /* Вывод информации о последовательностях обработки потоков
         * |  #1 Source[0:1] -> Decoder pcm_mulaw -> Resampler -> Encoder aac -> Sink[1:1]
         * |  #0 Source[0:0] -> Decoder flv -> Rescaler -> VideoFilter -> Encoder libx264 -> Sink[1:0]
        */
//        dump_str += "\nStream mapping:";
//        int64_t i = 0;
//        std::string delimeter = " -> ";
////        auto stream_map = _map->streamMap();
//        for (auto&& sequence : _processor_sequences) {
//            dump_str += "\n";
//            dump_str += TAB;
//            dump_str += "#" + std::to_string(i++) + " ";
//            for (auto&& elem : sequence) {
//                if (elem->is("YMap")) { continue; }
//                dump_str += elem->name();
//                if (elem->is("Source") || elem->is("Sink")) { //TODO устаревший код
//                    auto context = dynamic_cast<FormatContext*>(elem);
//                    dump_str += "[" + std::to_string(context->uid())
//                            + ":"
//                            + std::to_string(i-1) + "]"; //TODO stream_index, брать из _stream_map->streamMap();
//                }
//                if (elem->is("Decoder")) {
//                    dump_str += " " + std::string(static_cast<Decoder*>(elem)->decoderContext()->codec->name);
//                }
//                if (elem->is("Encoder")) {
//                    dump_str += " " + std::string(static_cast<Encoder*>(elem)->encoderContext()->codec->name);
//                }
//                dump_str += delimeter;
//            }
//            dump_str.erase(dump_str.size() - delimeter.size(), delimeter.size());
//        }

        return dump_str;
    }

    SharedStream Pipeline::findBestInputStream(MediaType type) {
        StreamVector stream_list;
        auto stream_getter = [type,&stream_list](const auto& source) {
            auto stream = source->bestStream(type);
            if (not_inited_ptr(stream)) { return; }
            stream_list.push_back(stream);
        };
        _data_sources.for_each(stream_getter);
        if (stream_list.empty()) {
            log_warning("Sources do not contain " << type << " stream, search among backup");
            _data_backup_sources.for_each(stream_getter);
        }
        return utils::find_best_stream(stream_list);
    }

    Route *Pipeline::findRoute(const int64_t uid) { //TODO
        for (auto&& route : _route_list) {
            if (route.contains(uid)) {
                return &route;
            }
        }
        return nullptr;
    }

} // namespace fpp
