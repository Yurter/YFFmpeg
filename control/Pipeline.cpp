#include "Pipeline.hpp"
#include <iterator>

namespace fpp {

    Pipeline::Pipeline() :
        _map(new YMap)
    {
        setName("Pipeline");
        _data_processors.push_back(_map);
    }

    Pipeline::~Pipeline() {
        freeProcesors();
        stop();
    }

    bool Pipeline::stop() {
        log_info("Stopping...");
        stopProcesors();
        closeContexts();
        joinProcesors();
        stop_log();
        log_info("Done");
        return true;
    }

    void Pipeline::pause() {
        _paused = true;
        log_info("Paused");
    }

    void Pipeline::unpause() {
        log_info("Unpaused");
        _paused = false;
    }

    void Pipeline::setOptions(int64_t options) {
        _options = options;
    }

    void Pipeline::addElement(Object* element) {
        _data_processors.push_back(element);
    }

    //void Pipeline::setRoute(Stream* input_stream, Stream* output_stream)
    //{
    //    _map->addRoute(input_stream, output_stream);
    //}

    //TODO
    void Pipeline::setRoute(Context* input_context, int64_t input_stream_index
                           , Context* output_context, int64_t output_stream_index) {
        _metamap.push_back({{ input_context->uid(), input_stream_index }
                        , { output_context->uid(), output_stream_index }});
    }

    void Pipeline::dump() const {
        log_info(toString());
    }

    Code Pipeline::init() {
        log_info("Initialization started...");
        try_to(checkIOContexts());
        try_to(initContext());
        try_to(openSources());
        try_to(initMap());
        try_to(openSinks());
        /* try_to(openContext()); */
        try_to(determineSequences());
        try_to(initRefi());
        try_to(initCodec());
        try_to(startProcesors());
        dump();
        setInited(true);
        return Code::OK;
    }

    Code Pipeline::run() {
        bool all_processor_stopped = true;
        for (auto&& processor : _data_processors) {
            auto thread_processor = static_cast<Thread*>(processor);
            if (thread_processor->running() && !thread_processor->is("YMap")) {
                log_info(thread_processor->name());
//                if (thread_processor->is("Decoder flv")) {
//                    log_info(dynamic_cast<AsyncQueue<Packet>*>(thread_processor));
//                }
                all_processor_stopped = false;
//                break;
            }
        }
        return_if(all_processor_stopped, Code::END_OF_FILE);
        utils::sleep_for(LONG_DELAY_MS);
        return Code::OK;
    }
//    Code Pipeline::run() {
//        for (auto&& processor : _data_processors) {
//            auto thread_processor = static_cast<Thread*>(processor);
//            return_if_not(thread_processor->running()
//                          , thread_processor->exitCode());
//        }
//        utils::sleep_for(LONG_DELAY_MS);
//        return Code::OK;
//    }

    bool Pipeline::option(Option option) const {
        return _options & option;
    }

    Code Pipeline::checkIOContexts() {
        return_if(sources().empty(),    Code::NOT_INITED);  //TODO throw YException("No source specified");
        return_if(sinks().empty(),      Code::NOT_INITED);  //TODO throw YException("No destination specified");
        return Code::OK;
    }

    Code Pipeline::initMap() {
        auto stream_map = _map->streamMap();
        if (stream_map->empty()) {
            /* Пользователь не установил таблицу маршрутов явно,
             * определяются маршруты по умолчанию */
            /* Проброс наилучшего видео-потока */
            try_to(connectIOStreams(MediaType::MEDIA_TYPE_VIDEO));
            /* Проброс наилучшего аудио-потока */
            try_to(connectIOStreams(MediaType::MEDIA_TYPE_AUDIO));
        }
        try_to(_map->init());
        log_debug(_map->toString());
        return Code::OK;
    }

    Code Pipeline::initRefi() {
        for (auto&& processor : _data_processors) {
            auto refi = dynamic_cast<Refi*>(processor);
            if (inited_ptr(refi)) { try_to(refi->init()); }
        }
        return Code::OK;
    }

    Code Pipeline::initCodec() {
        for (auto&& processor : _data_processors) {
            auto decoder = dynamic_cast<Decoder*>(processor);
            if (inited_ptr(decoder)) { try_to(decoder->init()); continue; }
            auto encoder = dynamic_cast<Encoder*>(processor);
            if (inited_ptr(encoder)) { try_to(encoder->init()); continue; }
        }
        return Code::OK;
    }

    Code Pipeline::initContext() {
        for (auto&& processor : _data_processors) {
            auto context = dynamic_cast<Context*>(processor);
            if (inited_ptr(context)) { try_to(context->init()); }
        }
        return Code::OK;
    }

    Code Pipeline::openSources() {
        for (auto&& processor : _data_processors) {
            auto source = dynamic_cast<Source*>(processor);
            if (inited_ptr(source)) { try_to(source->open()); }
        }
        return Code::OK;
    }

    Code Pipeline::openSinks() {
        for (auto&& processor : _data_processors) {
            auto sink = dynamic_cast<Sink*>(processor);
            if (inited_ptr(sink)) { try_to(sink->open()); }
        }
        return Code::OK;
    }

//    Code Pipeline::openContext() {
//        for (auto&& processor : _data_processors) {
//            auto context = dynamic_cast<Context*>(processor);
//            if (inited_ptr(context)) { try_to(context->open()); }
//        }
//        return Code::OK;
//    }

    Code Pipeline::closeContexts() {
        for (auto&& processor : _data_processors) {
            auto context = dynamic_cast<Context*>(processor);
            if (inited_ptr(context)) { try_to(context->close()); }
        }
        return Code::OK;
    }

    Code Pipeline::startProcesors() {
        for (auto&& processor : _data_processors) {
            auto thread_processor = static_cast<Thread*>(processor);
            try_to(thread_processor->start());
        }
        return Code::OK;
    }

    Code Pipeline::stopProcesors() {
        for (auto&& processor : _data_processors) {
            auto thread_processor = static_cast<Thread*>(processor);
            try_to(thread_processor->quit());
        }
        return Code::OK;
    }

    Code Pipeline::joinProcesors() {
        for (auto&& processor : _data_processors) {
            auto thread_processor = static_cast<Thread*>(processor);
            thread_processor->join();
        }
        return Code::OK;
    }

    Code Pipeline::determineSequences() {
        StreamMap* stream_map = _map->streamMap();
        return_if(stream_map->empty(), Code::NOT_INITED);
        for (auto&& stream_route : *stream_map) {
            Stream* in_stream = stream_route.first;
            Stream* out_stream = stream_route.second;
            StreamPair in_out_streams { in_stream, out_stream };
            ProcessorSequence sequence;

            sequence.push_back(in_stream->context());
            sequence.push_back(_map);

            bool processing_required =
                    (in_stream->isVideo() && !option(Option::COPY_VIDEO))
                    || (in_stream->isAudio() && !option(Option::COPY_AUDIO)) ;

            if (processing_required) {
                bool rescaling_required     = utils::rescaling_required(in_out_streams);
                bool resampling_required    = utils::resampling_required(in_out_streams);
                bool video_filter_required  = utils::video_filter_required(in_out_streams);
                bool audio_filter_required  = utils::audio_filter_required(in_out_streams);
                bool transcoding_required   = utils::transcoding_required(in_out_streams);

                transcoding_required = (transcoding_required
                                        || rescaling_required
                                        || resampling_required);

                if (transcoding_required) {
                    Decoder* decoder = new Decoder(in_stream);
                    sequence.push_back(decoder);
                    addElement(decoder);
                }

                if (rescaling_required) {
                    Rescaler* rescaler = new Rescaler(in_out_streams);
                    sequence.push_back(rescaler);
                    addElement(rescaler);
                }

                if (video_filter_required) {
//                    std::string filters_descr = "select='not(mod(n,10))',setpts=N/FRAME_RATE/TB";
//                    VideoFilter* video_filter = new VideoFilter(in_out_streams, filters_descr);
//                    sequence.push_back(video_filter);
//                    addElement(video_filter);
                }

                if (audio_filter_required) {
                    //
                }

                if (resampling_required) {
                    Resampler* resampler = new Resampler(in_out_streams);
                    sequence.push_back(resampler);
                    addElement(resampler);
                }

                if (transcoding_required) {
                    Encoder* encoder = new Encoder(out_stream);
                    sequence.push_back(encoder);
                    addElement(encoder);
                }
            }

            sequence.push_back(out_stream->context());

            for (auto processor_it = sequence.begin(); processor_it != sequence.end(); processor_it++) {
                auto next_processor_it = std::next(processor_it);
                if (next_processor_it == sequence.end()) { break; }
                log_debug((*processor_it)->name() + " connected to " + (*next_processor_it)->name());
                if ((*processor_it)->is("Source")) {
                    try_to(static_cast<Source*>(*processor_it)->connectOutputTo(*next_processor_it));
                } else if ((*processor_it)->is("Encoder")) {
                    try_to(static_cast<Encoder*>(*processor_it)->connectOutputTo(*next_processor_it));
                } else if ((*processor_it)->is("Decoder")) {
                    try_to(static_cast<Decoder*>(*processor_it)->connectOutputTo(*next_processor_it));
                } else if ((*processor_it)->is("Rescaler")) {
                    try_to(static_cast<Rescaler*>(*processor_it)->connectOutputTo(*next_processor_it));
                } else if ((*processor_it)->is("Resampler")) {
                    try_to(static_cast<Resampler*>(*processor_it)->connectOutputTo(*next_processor_it));
                } else if ((*processor_it)->is("VideoFilter")) {
                    try_to(static_cast<VideoFilter*>(*processor_it)->connectOutputTo(*next_processor_it));
                }else {
                    log_warning("didn't connected: " + (*processor_it)->name());
                }
            }

            auto it = sequence.begin();
            std::advance(it, 2);
            auto first_packet_processor = *it;
            _map->setRoute(in_stream, dynamic_cast<PacketProcessor*>(first_packet_processor));
            _processor_sequences.push_back(sequence);
        }

        return Code::OK;
    }

    void Pipeline::freeProcesors() //TODO объекты в смартпоинтеры
    {
        //см TODO ↗
    }

    std::string Pipeline::toString() const {
        std::string dump_str;

        /* Вывод информации о контекстах
         * Input #0, rtsp, from 'rtsp://admin:admin@192.168.10.3':
         *      Stream #0:0: Video: h264, 1920x1080 [16:9], 23 fps, 400 kb/s
         *      Stream #0:1: Audio: pcm_mulaw, 16000 Hz, 1 channels, s16, 128 kb/s
         * Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/ytub-8t5w-asjj-avyf':
         *      Stream #0:0: Video: h264, 1920x1080, 400 kb/s
         *      Stream #0:1: Audio: aac, 44100 Hz, stereo, 128 kb/s
        */
        for (auto&& context : contexts()) {
            dump_str += "\n" + context->toString();
            for (auto i = 0; i < context->numberStream(); i++) {
                dump_str += "\n";
                dump_str += TAB + context->stream(i)->toString(); //TODO tut
            }
        }

        /* Вывод информации о последовательностях обработки потоков */
        dump_str += "\nProcessing sequences";
        int64_t i = 0;
        std::string delimeter = " -> ";
    //    auto& stream_map = _stream_map->streamMap();
        //TODO
        // #1 Source[0:1] -> Decoder pcm_mulaw -> Resampler -> Encoder aac -> Sink[1:1]
        for (auto&& sequence : _processor_sequences) {
            dump_str += "\n";
            dump_str +=  TAB;
            dump_str += "#" + std::to_string(i++) + " ";
            for (auto&& elem : sequence) {
                if (elem->is("YMap")) { continue; }
                dump_str += elem->name();
                if (elem->is("Source") || elem->is("Sink")) {
                    auto context = dynamic_cast<Context*>(elem);
                    dump_str += "[" + std::to_string(context->uid())
                            + ":"
                            + std::to_string(i-1) + "]"; //TODO stream_index, брать из _stream_map->streamMap();
                }
                dump_str += delimeter;
            }
            dump_str.erase(dump_str.size() - delimeter.size(), delimeter.size());
        }

        return dump_str;
    }

    Stream* Pipeline::findBestInputStream(MediaType media_type) {
        switch (media_type) {
        case MediaType::MEDIA_TYPE_VIDEO: {
            StreamVector all_video_streams;
            for (auto&& source : sources()) {
                all_video_streams.push_back(source->bestStream(MediaType::MEDIA_TYPE_VIDEO));
            }
            return static_cast<VideoStream*>(utils::find_best_stream(all_video_streams));
        }
        case MediaType::MEDIA_TYPE_AUDIO: {
            StreamVector all_audio_streams;
            for (auto&& source : sources()) {
                all_audio_streams.push_back(source->bestStream(MediaType::MEDIA_TYPE_AUDIO));
            }
            return static_cast<AudioStream*>(utils::find_best_stream(all_audio_streams));
        }
        default:
            return nullptr;
        }
    }

    StreamList Pipeline::getOutputStreams(MediaType media_type) {
        StreamList output_streams;
        for (auto&& sink : sinks()) {
            for (auto&& video_stream : sink->streams(media_type)) {
                output_streams.push_back(video_stream);
            }
        }
        return output_streams;
    }

    Code Pipeline::connectIOStreams(MediaType media_type) {
        Stream* best_stream = findBestInputStream(media_type);
        if (not_inited_ptr(best_stream)) {
            log_warning(utils::media_type_to_string(media_type)
                        << " stream is not present in the sources");

            return Code::OK; //TODO ?? что возращать? //        return Code::INVALID_INPUT; ?
        }
        StreamList output_streams = getOutputStreams(media_type);
        if (output_streams.empty()) {
            log_warning("Sinks does not have any "
                        << utils::media_type_to_string(media_type)
                        << " stream");
            log_info("Creating " << utils::media_type_to_string(media_type) << " stream in every sink...");
            for (auto&& sink : sinks()) {
                try_to(sink->createStream(best_stream->parameters));
                log_info("Created "
                         << utils::media_type_to_string(media_type)
                         << " stream in "
                         << sink->mediaResourceLocator());
            }
            output_streams = getOutputStreams(media_type);
        }
        /* Трансляция наилучшего потока на все потоки выхода того же медиа-типа */
        for (auto&& out_stream : output_streams) {
            try_to(_map->addRoute(best_stream, out_stream));
            best_stream->setUsed(true);
            out_stream->setUsed(true);
        }
        return Code::OK;
    }

    ContextList Pipeline::contexts() const {
        ContextList context_list;
        for (auto&& processor : _data_processors) {
            auto context = dynamic_cast<Context*>(processor);
            if (inited_ptr(context)) { context_list.push_back(context); }
        }
        return context_list;
    }

    SourceList Pipeline::sources() const {
        SourceList source_list;
        for (auto&& processor : _data_processors) {
            auto source = dynamic_cast<Source*>(processor);
            if (inited_ptr(source)) { source_list.push_back(source); }
        }
        return source_list;
    }

    SinkList Pipeline::sinks() const {
        SinkList sink_list;
        for (auto&& processor : _data_processors) {
            auto sink = dynamic_cast<Sink*>(processor);
            if (inited_ptr(sink)) { sink_list.push_back(sink); }
        }
        return sink_list;
    }

    DecoderList Pipeline::decoders() const {
        DecoderList decoder_list;
        for (auto&& processor : _data_processors) {
            auto decoder = dynamic_cast<Decoder*>(processor);
            if (inited_ptr(decoder)) { decoder_list.push_back(decoder); }
        }
        return decoder_list;
    }

} // namespace fpp
