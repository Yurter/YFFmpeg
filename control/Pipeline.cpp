#include "Pipeline.hpp"
#include <iterator>

namespace fpp {

    Pipeline::Pipeline()
    {
        setName("Pipeline");
    }

    Pipeline::~Pipeline() {
        try_throw(stop());
        join();
    }

    Code Pipeline::addElement(ProcessorPointer processor) { //TODO каша из кода
        switch (processor->type()) {
        case ProcessorType::Input:
            try_to(processor->init());
            try_to(processor->open());
            _data_sources.push_back(processor);
            return Code::OK;
        case ProcessorType::Output:
            _data_sinks.push_back(processor);
            try_to(processor->init());
            try_to(determineSequence(processor));
            return Code::OK;
        case ProcessorType::Process:
            return Code::NOT_IMPLEMENTED;
        case ProcessorType::Unknown:
            return Code::INVALID_INPUT;
        }
        return Code::ERR;
    }

    void Pipeline::remElement(ProcessorPointer processor) {
        _data_sinks.remove_if([processor](const auto& sink) {
            return sink->uid() == processor->uid();
        });
        _data_sources.remove_if([processor](const auto& source) {
            return source->uid() == processor->uid();
        });
    }

    void Pipeline::remElement(const int64_t uid) {
        _data_sinks.remove_if([uid](const auto& sink) { return sink->uid() == uid; });
        _data_sources.remove_if([uid](const auto& source) { return source->uid() == uid; });

        findRoute(uid)->destroy();
        auto cond = [uid](const Route& route){ return route.contains(uid); };
        _route_list.erase(std::remove_if(_route_list.begin(), _route_list.end(), cond), _route_list.end());
    }

    void Pipeline::dump() const {
        log_info(toString());
    }

    Code Pipeline::init() {
        log_info("Initialization started...");
        try_to(checkFormatContexts());  /* Проверка на наличие входо-выходов    */
        dump();                         /* Дамп всей информации в лог           */
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
//        try_to(stopProcesors());
        _data_sources.for_each([](auto& source){
            try_throw_static(source->stop());
        });
        log_info("Processing finished");
        return Code::OK;
    }

    Code Pipeline::checkFormatContexts() { //TODO
        return Code::OK; //debug
        return_if(_data_sources.empty(),   Code::NOT_INITED);
//        return_if(mediaSinks().empty(),     Code::NOT_INITED);
        return Code::OK;
    }

//    Code Pipeline::stopProcesors() { //TODO
//        _data_sinks.for_each([](const auto& sink) {
//            try_throw_static(sink->stop());
//        });
//        return Code::OK;
//    }

    Code Pipeline::joinProcesors() {
        _data_sources.for_each([](auto& source) {
            try_throw_static(source->stop());
        });
        _data_sinks.for_each([](auto& sink) {
            try_throw_static(sink->stop());
        });
        return Code::OK;
    }

    Code Pipeline::createSequence(Route& route) {
        Stream* input_stream = findStream(route.inputStreamUid());
        Stream* output_stream = findStream(route.outputStreamUid());
        return_error_if(not_inited_ptr(input_stream)
                        , "Failed to find input stream"
                        , Code::ERR);
        return_error_if(not_inited_ptr(output_stream)
                        , "Failed to find output stream"
                        , Code::ERR);

        const IOParams params { input_stream->params, output_stream->params };

        try_to(output_stream->params->completeFrom(input_stream->params));

        input_stream->setUsed(true);
        output_stream->setUsed(true);

//        { //TODO не универсальый каст
//            auto source_ptr = dynamic_cast<Processor*>(input_stream->context());
//            return_error_if(not_inited_ptr(source_ptr)
//                            , "Failed to cast input stream's context to Processor."
//                            , Code::INVALID_INPUT);
//            //TODO проблема двойного освобождения указателя
////            ProcessorPointer source(source_ptr);
////            ProcessorPointer source = std::make_shared<MediaSource>(source_ptr);
//            ProcessorPointer source = std::make_shared<MediaSource>(*source_ptr);
//            try_to(route.append(source));
//        }
        try_to(route.append(findProcessorByUid(input_stream->params->contextUid())));
//        try_to(route.append(input_stream->context()));

        bool rescaling_required     = utils::rescaling_required   (params);
        bool resampling_required    = utils::resampling_required  (params);
        bool video_filter_required  = utils::video_filter_required(params);
        bool audio_filter_required  = utils::audio_filter_required(params);
        bool transcoding_required   = utils::transcoding_required (params);

        auto debug = findProcessorByUid(output_stream->params->contextUid());
        auto out_context = dynamic_cast<MediaSink*>(findProcessorByUid(output_stream->params->contextUid()).get()); //TODO кривой код: вынести IOType в Processor ?
        if (inited_ptr(out_context)) {
            video_filter_required = video_filter_required
                                    || out_context->outputFormatContext()->preset(IOType::Timelapse);
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
//        if (inited_ptr(dynamic_cast<OpenCVSink*>(output_stream->context()))) { //TODO кривой код
//            encoding_required = false;
//        }

        if (decoding_required) {
            ProcessorPointer decoder = std::make_shared<Decoder>(params);
            try_to(route.append(decoder));
        }

        if (rescaling_required) {
            ProcessorPointer rescaler = std::make_shared<Rescaler>(params);
            try_to(route.append(rescaler));
        }

        if (video_filter_required) {
//            std::string filters_descr = "select='not(mod(n,10))',setpts=N/FRAME_RATE/TB";
//            std::string filters_descr = "setpts=N/(10*TB)";
//            std::string filters_descr = "setpts=0.016*PTS";
            std::string filters_descr = "select='not(mod(n,60))'";
            ProcessorPointer video_filter = std::make_shared<VideoFilter>(params, filters_descr);
            try_to(route.append(video_filter));
        }

        if (audio_filter_required) {
            return Code::NOT_IMPLEMENTED;
        }

        if (resampling_required) {
            ProcessorPointer resampler = std::make_shared<Resampler>(params);
            try_to(route.append(resampler));
        }

        if (encoding_required) {
            ProcessorPointer encoder = std::make_shared<Encoder>(params);
            try_to(route.append(encoder));
        }

//        { //TODO не универсальый каст
//            auto sink_ptr = dynamic_cast<Processor*>(output_stream->context());
//            return_error_if(not_inited_ptr(sink_ptr)
//                            , "Failed to cast output stream's context to Processor."
//                            , Code::INVALID_INPUT);
//            //TODO проблема двойного освобождения указателя
////            ProcessorPointer sink(sink_ptr);
////            ProcessorPointer sink = std::make_shared<MediaSink>(sink_ptr);
//            ProcessorPointer sink = std::make_shared<MediaSink>(*sink_ptr);
//            try_to(route.append(sink));
//        }
        try_to(route.append(findProcessorByUid(output_stream->params->contextUid())));

        _route_list.push_back(route);
//        try_to(simplifyRoutes());
        try_to(route.init());
        try_to(route.startAll());

        log_info("route: " << route);

        return Code::OK;
    }

    ProcessorPointer Pipeline::findProcessorByUid(const int64_t uid) {
        ProcessorPointer result;
        _data_sources.for_each([uid,&result](const auto& source) {
//            static_log_error("sources", "uid: " << uid << ", source->uid(): " << source->uid());
            if (source->uid() == uid) {
                result = source;
            }
        });
        _data_sinks.for_each([uid,&result](const auto& sink) {
//            static_log_error("sinks", "uid: " << uid << ", sink->uid(): " << sink->uid());
            if (sink->uid() == uid) {
                result = sink;
            }
        });
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
//    Code Pipeline::simplifyRoutes() {
//        return_if(_route_list.empty(), Code::OK);
//        for (size_t i = 0; i < (_route_list.size() - 1); ++i) {
//            for (size_t j = i + 1; j < _route_list.size(); ++j) {
//                Route& route_one = _route_list[i];
//                Route& route_two = _route_list[j];

//                log_warning("");
//                log_warning("Comparing ");
//                log_warning(i << "] " << route_one);
//                log_warning(j << "] " << route_two);

//                auto sequence_one = route_one.processorSequence(); //TODO заменить на сслыки или указатели
//                auto sequence_two = route_two.processorSequence();

//                size_t min_size = std::min(sequence_one.size(), sequence_two.size());

//                for (size_t k = 0; k < min_size; ++k) {
//                    if (k > 0) {
//                        if (sequence_one[k]->uid() == sequence_two[k]->uid()) {
//                            //уже один и тот же
//                            break;
//                        }
//                    }
//                    if_not(sequence_one[k]->equalTo(sequence_two[k])) {
//                        log_warning("FORK POINT is " << sequence_one[k]->name());
//                        //TODO склеить последовательности от 0 до k-1
//                        if (k > 0) {
////                            ProcessorVector mutual(route_one.processorSequence().begin()
////                                                   , route_one.processorSequence().begin() + int64_t(k - 1));
//                            ProcessorVector mutual;
//                            for (size_t j = 0; j < k; ++j) {
//                                mutual.push_back(route_one.processorSequence()[j]);
//                            }
//                            try_to(route_two.changePartTo(mutual));
//                        }
//                        break;
//                    }
//                }
//            }
//        }
//        for (const auto& route : _route_list) {
//            log_warning("Result: " << route);
//        }
//        return Code::OK;
//    }

    Stream* Pipeline::findStream(int64_t uid) {
        Stream* ret_stream = nullptr;
        _data_sources.for_each([uid,&ret_stream](const auto& source) {
            for (auto&& stream : source->streams()) {
                if (stream->params->streamUid() == uid) {
                    ret_stream = stream;
                    return;
                }
            }
        });
        _data_sinks.for_each([uid,&ret_stream](const auto& sink) {
            for (auto&& stream : sink->streams()) {
                if (stream->params->streamUid() == uid) {
                    ret_stream = stream;
                    return;
                }
            }
        });
        return ret_stream;

        ////////////////////////////////////////////////////////////
//        for (const auto& source : _data_sources) {
//            for (auto&& stream : source->streams()) {
//                if (stream->parameters->streamUid() == uid) {
//                    return stream;
//                }
//            }
//        }
//        for (auto&& sink : _data_sinks) {
//            for (auto&& stream : sink->streams()) {
//                if (stream->parameters->streamUid() == uid) {
//                    return stream;
//                }
//            }
//        }
        return nullptr;
    }

    Code Pipeline::determineSequence(const ProcessorPointer output_processor) {
        const auto& output_streams = output_processor->streams();
        if (output_streams.empty()) {
            log_error(output_processor->name() << " doesn't has any stream.");
            return Code::NOT_INITED;
        }
        for (auto out_stream : output_streams) {
            Stream* in_stream = findBestInputStream(out_stream->type());
            if (not_inited_ptr(in_stream)) {
                log_error("Failed to find input stream type "
                          << utils::media_type_to_string(out_stream->type()));
                return Code::INVALID_INPUT;
            }
            Route route;
            try_to(route.setMetaRoute(in_stream->params->streamUid()
                                      , out_stream->params->streamUid()));
            try_to(createSequence(route));
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

    Stream* Pipeline::findBestInputStream(MediaType media_type) { //TODO
        switch (media_type) {
        case MediaType::MEDIA_TYPE_VIDEO: {
            StreamVector all_video_streams;
            _data_sources.for_each([media_type,&all_video_streams](const auto& source) {
                UNUSED(media_type); //TODO
                all_video_streams.push_back(source->stream(0));
            });
            return static_cast<VideoStream*>(utils::find_best_stream(all_video_streams));
        }
        case MediaType::MEDIA_TYPE_AUDIO: {
//            StreamVector all_audio_streams;
//            for (auto&& source : mediaSinks()) {
//                all_audio_streams.push_back(source->outputFormatContext().bestStream(MediaType::MEDIA_TYPE_AUDIO));
//            }
//            return static_cast<AudioStream*>(utils::find_best_stream(all_audio_streams));
            return nullptr;
        }
        default:
            return nullptr;
        }
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
