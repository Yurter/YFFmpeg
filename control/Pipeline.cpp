#include "Pipeline.hpp"
#include <iterator>

namespace fpp {

    Pipeline::Pipeline()
    {
        setName("Pipeline");
    }

    Pipeline::~Pipeline() {
        try_throw(stop());
    }

    Code Pipeline::addElement(FrameSource* frame_source) {
        try_to(frame_source->init());
        try_to(frame_source->open());
        std::lock_guard lock(_processor_mutex);
        _data_sources.push_back(frame_source);
        return Code::OK;
    }

    Code Pipeline::addElement(FrameSink* frame_sink) {
        std::lock_guard lock(_processor_mutex);
        _data_sinks.push_back(frame_sink);
        try_to(frame_sink->init());
        try_to(determineSequence(frame_sink));
        return Code::OK;
    }

    Code Pipeline::addElement(PacketSource* packet_source) {
        try_to(packet_source->init());
        try_to(packet_source->open());
        std::lock_guard lock(_processor_mutex);
        _data_sources.push_back(packet_source);
        return Code::OK;
    }

    Code Pipeline::addElement(PacketSink* packet_sink) {
        try_to(packet_sink->init());
        std::lock_guard lock(_processor_mutex);
        _data_sinks.push_back(packet_sink);
        try_to(determineSequence(packet_sink));
        return Code::OK;
    }

    void Pipeline::setOptions(int64_t options) {
        UNUSED(options);
//        _options = options;
    }

    void Pipeline::remElement(Processor* processor) {
        std::lock_guard lock(_processor_mutex);
        _data_sinks.remove_if([processor](auto proc) { return proc == processor; });
        _data_sources.remove_if([processor](auto proc) { return proc == processor; });

        findRoute(processor).destroy();
//        _route_list.remove_if([processor](const Route& route){ return route.contains(processor); });
        auto cond = [processor](const Route& route){ return route.contains(processor); };
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

    Code Pipeline::run() {
        bool all_processor_stopped = true;
        for (auto&& processor : _data_sources) {
            auto thread_processor = static_cast<Thread*>(processor);
            /* Прекращение работы, если процессор завершил работу с ошибкой */
            return_if(utils::error_code(thread_processor->exitCode())
                      , thread_processor->exitCode());
            if (thread_processor->running()) {
                all_processor_stopped = false;
                break;
            } else {
                /* Выброс отработавшего процессора из пула */
            }
        }
        for (auto&& processor : _data_sinks) {
            auto thread_processor = static_cast<Thread*>(processor);
            /* Прекращение работы, если процессор завершил работу с ошибкой */
            return_if(utils::error_code(thread_processor->exitCode())
                      , thread_processor->exitCode());
            if (thread_processor->running()) {
                all_processor_stopped = false;
                break;
            } else {
                /* Выброс отработавшего процессора из пула */
            }
        }
        if (all_processor_stopped) {
            int debug_stop = 0;
        }
        return_if(all_processor_stopped, Code::END_OF_FILE);
        utils::sleep_for(LONG_DELAY_MS/* * 10*/);
        return Code::OK;
    }

    Code Pipeline::onStart() {
        // TODO
        return Code::OK;
    }

    Code Pipeline::onStop() {
        log_info("Stopping...");
        try_to(stopProcesors());
        log_info("Processing finished.");
        return Code::OK;
    }

    bool Pipeline::option(Option option) const {
        UNUSED(option);
        return false;
//        return _options & option;
    }

    Code Pipeline::checkFormatContexts() {
        return Code::OK; //debug
        return_if(_data_sources.empty(),   Code::NOT_INITED);
//        return_if(mediaSinks().empty(),     Code::NOT_INITED);
        return Code::OK;
    }

    Code Pipeline::stopProcesors() {
        for (auto&& processor : _data_sinks) {
            auto thread_processor = static_cast<Thread*>(processor);
            log_error("Thread: " << thread_processor->name());
            try_to(thread_processor->stop());
        }
        return Code::OK;
    }

    void Pipeline::freeProcesors() //TODO объекты в смартпоинтеры
    {
        //см TODO ↗
//        for (auto&& proc : _processors) {
//            delete proc;
        //        }
    }

    Code Pipeline::createSequence(Route& route) {
        Stream* input_stream = findStream(route.inputStreamUid());
        Stream* output_stream = findStream(route.outputStreamUid());
        StreamPair in_out_streams { input_stream, output_stream };

        output_stream->parameters->completeFrom(input_stream->parameters);
        input_stream->setUsed(true);
        output_stream->setUsed(true);

        auto custom_ptr = dynamic_cast<CustomPacketSource*>(input_stream->context());
        if (inited_ptr(custom_ptr)) {
            try_to(route.append(custom_ptr));
        } else {
            try_to(route.append(static_cast<Processor*>(static_cast<FormatContext*>(input_stream->context())->_media_ptr)));
        }

        bool rescaling_required     = utils::rescaling_required   (in_out_streams);
        bool resampling_required    = utils::resampling_required  (in_out_streams);
        bool video_filter_required  = utils::video_filter_required(in_out_streams);
        bool audio_filter_required  = utils::audio_filter_required(in_out_streams);
        bool transcoding_required   = utils::transcoding_required (in_out_streams);

        auto out_context = dynamic_cast<OutputFormatContext*>(output_stream->context());
        if (inited_ptr(out_context)) {
            video_filter_required = video_filter_required
                                    || out_context->preset(IOType::Timelapse);
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
        if (inited_ptr(dynamic_cast<OpenCVSink*>(output_stream->context()))) {
            encoding_required = false;
        }

        if (decoding_required) {
            Decoder* decoder = new Decoder(input_stream);
            try_to(route.append(decoder));
//            try_to(addElement(decoder));
        }

        if (rescaling_required) {
            Rescaler* rescaler = new Rescaler(in_out_streams);
            try_to(route.append(rescaler));
//            try_to(addElement(rescaler));
        }

        if (video_filter_required) {
//                    std::string filters_descr = "select='not(mod(n,10))',setpts=N/FRAME_RATE/TB";
//                    std::string filters_descr = "setpts=N/(10*TB)";
            std::string filters_descr = "select='not(mod(n,60))'";
//            std::string filters_descr = "setpts=0.016*PTS";
            VideoFilter* video_filter = new VideoFilter(output_stream->parameters, filters_descr);
            try_to(route.append(video_filter));
//            try_to(addElement(video_filter));
        }

        if (audio_filter_required) {
            return Code::NOT_IMPLEMENTED;
        }

        if (resampling_required) {
            Resampler* resampler = new Resampler(in_out_streams);
            try_to(route.append(resampler));
//            try_to(addElement(resampler));
        }

        if (encoding_required) {
            Encoder* encoder = new Encoder(output_stream);
            try_to(route.append(encoder));
//            try_to(addElement(encoder));
        }

//        try_to(route.append(static_cast<Processor*>(static_cast<FormatContext*>(output_stream->context())->_media_ptr)));
        auto custom_ptr_out = dynamic_cast<CustomPacketSink*>(output_stream->context());
        if (inited_ptr(custom_ptr_out)) {
            try_to(route.append(custom_ptr_out));
        } else {
            try_to(route.append(static_cast<Processor*>(static_cast<FormatContext*>(output_stream->context())->_media_ptr)));
        }

        try_to(route.init());
        try_to(route.startAll());

        log_info("route: " << route);

        return Code::OK;
    }

    Code Pipeline::simplifyRoutes() {
        for (size_t i = 0; i < (_route_list.size() - 1); ++i) {
            for (size_t j = i + 1; j < _route_list.size(); ++j) {
                Route& route_one = _route_list[i];
                Route& route_two = _route_list[j];

                log_warning("");
                log_warning("Comparing ");
                log_warning(i << "] " << route_one);
//                log_warning("VS");
                log_warning(j << "] " << route_two);

                auto sequence_one = route_one.processorSequence();
                auto sequence_two = route_two.processorSequence();

                size_t min_size = std::min(sequence_one.size(), sequence_two.size());

                for (size_t k = 0; k < min_size; ++k) {
                    if_not(sequence_one[k]->equalTo(sequence_two[k])) {
                        log_warning("FORK POINT is " << sequence_one[k]->name());
                        break;
                    }
                }
            }
        }
        return Code::NOT_IMPLEMENTED;
    }

    Stream* Pipeline::findStream(int64_t uid) {
        int64_t context_uid = utils::get_context_uid(uid);
        for (auto&& source : _data_sources) {
            if (source->is("MediaSource")) {
                if (static_cast<MediaSource*>(source)->inputFormatContext()->uid() == context_uid) {
                    for (auto&& stream : static_cast<MediaSource*>(source)->inputFormatContext()->streams()) {
                        if (stream->uid() == uid) {
                            return stream;
                        }
                    }
                }
            }
            if (source->is("CustomPacketSource")) {
                for (auto&& stream : static_cast<CustomPacketSource*>(source)->streams()) {
                    if (stream->uid() == uid) {
                        return stream;
                    }
                }
            }
        }
        for (auto&& sink : _data_sinks) {
            if (sink->is("MediaSink")) {
                if (static_cast<MediaSink*>(sink)->outputFormatContext().uid() == context_uid) {
                    for (auto&& stream : static_cast<MediaSink*>(sink)->outputFormatContext().streams()) {
                        if (stream->uid() == uid) {
                            return stream;
                        }
                    }
                }
            }
            if (sink->is("CustomPacketSink")) {
                for (auto&& stream : static_cast<CustomPacketSink*>(sink)->streams()) {
                    if (stream->uid() == uid) {
                        return stream;
                    }
                }
            }
        }
        return nullptr;
    }

//    Code Pipeline::determineSequence(Processor* output_processor) {
//        StreamVector output_streams;
//        if (output_processor->is("MediaSink")) {
//            MediaSink* media_sink = static_cast<MediaSink*>(output_processor);
//            output_streams = media_sink->outputFormatContext().streams();
//            if (output_streams.empty()) {
//                log_error("output_streams is empty: " << media_sink->outputFormatContext().mediaResourceLocator());
//                return Code::NOT_INITED;
//            }
//            for (auto out_stream : output_streams) {
//                Stream* in_stream = findBestInputStream(out_stream->type());
//                if (not_inited_ptr(in_stream)) {
//                    log_error("Failed to find input stream type " << out_stream->type());
//                    return Code::INVALID_INPUT;
//                }
//                Route route;
//                try_to(route.setMetaRoute(in_stream->uid(), out_stream->uid()));
//                try_to(createSequence(route));
//                _route_list.push_back(route);
//            }
//            return Code::OK;
//        }
//        if (output_processor->is("CustomPacketSink")) {
//            CustomPacketSink* custom_sink = static_cast<CustomPacketSink*>(output_processor);
//            output_streams = custom_sink->streams();
//            if (output_streams.empty()) {
//                log_error("output_streams is empty: " << custom_sink);
//                return Code::NOT_INITED;
//            }
//            for (auto out_stream : output_streams) {
//                Stream* in_stream = findBestInputStream(out_stream->type());
//                if (not_inited_ptr(in_stream)) {
//                    log_error("Failed to find input stream type " << out_stream->type());
//                    return Code::INVALID_INPUT;
//                }
//                Route route;
//                try_to(route.setMetaRoute(in_stream->uid(), out_stream->uid()));
//                try_to(createSequence(route));
//                _route_list.push_back(route);
//            }
//            return Code::OK;
//        }
//        return Code::NOT_IMPLEMENTED;
//    }

    Code Pipeline::determineSequence(Processor* output_processor) {
        StreamVector output_streams = output_processor->streams();
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
            try_to(route.setMetaRoute(in_stream->uid(), out_stream->uid()));
            try_to(createSequence(route));
            _route_list.push_back(route);
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

    Stream* Pipeline::findBestInputStream(MediaType media_type) {
        switch (media_type) {
        case MediaType::MEDIA_TYPE_VIDEO: {
            StreamVector all_video_streams;
            for (auto&& source : _data_sources) {
                all_video_streams.push_back(source->stream(0)); //TODO !!! поиск по индексу, а не по типу
//                auto medias_source = dynamic_cast<MediaSource*>(source);
//                if (inited_ptr(medias_source)) {
//                    all_video_streams.push_back(medias_source->inputFormatContext().bestStream(MediaType::MEDIA_TYPE_VIDEO));
//                }
//                auto custom_source = dynamic_cast<CustomPacketSource*>(source);
//                if (inited_ptr(custom_source)) {
//                    all_video_streams.push_back(custom_source->stream(MediaType::MEDIA_TYPE_VIDEO));
//                }
            }
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

    Route Pipeline::findRoute(Processor* processor) {
        for (auto&& route : _route_list) {
            if (route.contains(processor)) {
                return route;
            }
        }
        return Route();
    }

} // namespace fpp
