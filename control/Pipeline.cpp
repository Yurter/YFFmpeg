#include "Pipeline.hpp"
#include <iterator>

namespace fpp {

    Pipeline::Pipeline() //:
//        _map(new YMap)
    {
        setName("Pipeline");
//        _processors.push_back(_map);
    }

    Pipeline::~Pipeline() {
//        try_to(stopProcesors());
//        freeProcesors();
//        stopPr();
        stop();
    }

    bool Pipeline::stopPr() {
        log_info("Stopping...");
        try_to(stopProcesors());
//        try_to(closeMediaSources());
//        try_to(closeMediaSinks());
//        try_to(joinProcesors());
//        try_to(stop_log());
        log_info("Processing finished.");
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

    Code Pipeline::addElement(Processor* processor) {
        _processors.push_back(processor);
        try_to(processor->init());
        if (processor->typeIs(ProcessorType::Input)) {
            try_to(processor->open());
        }
        if (processor->typeIs(ProcessorType::Output)) {
            try_to(determineSequence(processor));
            try_to(processor->open());
            processor->start();
        }
//        if (running()) {
//            if (processor->typeIs(ProcessorType::Output)) {
//                try_to(determineSequence(processor));
//            } else {
//                log_warning("Сannot be correctly added while running: " << processor->name());
//            }
//        }
        return Code::OK;
    }

    void Pipeline::remElement(Processor* processor) {
//        auto thread_processor = dynamic_cast<Thread*>(element); // нужно ли?
//        if (inited_ptr(thread_processor)) {
//            try_throw(thread_processor->stop());
//        }
        _processors.remove(processor);
        findRoute(processor).destroy();
        _route_list.remove_if([processor](const Route& route){ return route.contains(processor); });
    }

    //void Pipeline::setRoute(Stream* input_stream, Stream* output_stream)
    //{
    //    _map->addRoute(input_stream, output_stream);
    //}

    //TODO
    void Pipeline::setRoute(MediaSource* input_context, int64_t input_stream_index
                            , MediaSink* output_context, int64_t output_stream_index) {
//        _metamap.push_back({{ input_context->inputFormatContext().uid(), input_stream_index }
//                        , { output_context->outputFormatContext().uid(), output_stream_index }});
    }

    void Pipeline::dump() const {
        log_info(toString());
    }

    Code Pipeline::init() {
        log_info("Initialization started...");
        try_to(checkFormatContexts());  /* Проверка на наличие входо-выходов                            */
//        try_to(initMedia());            /* Инициализация форматКонтекстов                               */
//        try_to(openMediaSources());     /* Открытие входов и формирование входных потоков               */
//        try_to(initMap());              /* Автоматический проброс потоков                               */
//        try_to(openMediaSinks());       /* Открытие выходов                                             */
//        try_to(determineSequences());   /* Формирование поледовательностей обработки входных потоков    */
//        try_to(initRefi());             /* Инициализация рефи                                           */
//        try_to(initCodec());            /* Инициализация кодеков                                        */
//        try_to(openCodec());            /* Открытие кодеков                                             */
        try_to(startProcesors());       /* Запуск всех процессоров                                      */
        dump();                         /* Дамп всей информации в лог                                   */
        setInited(true);
//        log_warning(_map->toString());
        log_info("Processing started...");
        return Code::OK;
    }

    Code Pipeline::run() {
        bool all_processor_stopped = true;
        for (auto&& processor : _processors) {
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
//        try_to(closeMediaSources());
//        try_to(closeMediaSinks());
//        try_to(joinProcesors());
//        try_to(stop_log());
        log_info("Processing finished.");
        return Code::OK;
    }

    bool Pipeline::option(Option option) const {
        return _options & option;
    }

    Code Pipeline::checkFormatContexts() {
        return Code::OK; //debug
        return_if(mediaSources().empty(),   Code::NOT_INITED);  //TODO throw YException("No source specified");
//        return_if(mediaSinks().empty(),     Code::NOT_INITED);  //TODO throw YException("No destination specified");
        return Code::OK;
    }

//    Code Pipeline::initMap() {
//        auto stream_map = _map->streamMap();
//        if (stream_map->empty()) {
//            /* Пользователь не установил таблицу маршрутов явно,
//             * определяются маршруты по умолчанию */
//            /* Проброс наилучшего видео-потока */
//            try_to(connectIOStreams(MediaType::MEDIA_TYPE_VIDEO));
//            /* Проброс наилучшего аудио-потока */
//            try_to(connectIOStreams(MediaType::MEDIA_TYPE_AUDIO));
//        }
//        try_to(_map->init());
//        log_debug(_map->toString());
//        return Code::OK;
//    }

    Code Pipeline::initRefi() {
        for (auto&& processor : _processors) {
            auto refi = dynamic_cast<FrameProcessor*>(processor);
            if (inited_ptr(refi)) { try_to(refi->init()); }
        }
        return Code::OK;
    }

    Code Pipeline::initCodec() {
        for (auto&& decoder : decoders()) { try_to(decoder->init()); }
        for (auto&& encoder : encoders()) { try_to(encoder->init()); }
        return Code::OK;
    }

    Code Pipeline::initMedia() {
        for (auto&& source : mediaSources()) { try_to(source->init()); }
        for (auto&& sink   : mediaSinks())   { try_to(sink->init());   }
        return Code::OK;
    }

    Code Pipeline::openCodec() {
        for (auto&& decoder : decoders()) { try_to(decoder->open()); }
        for (auto&& encoder : encoders()) { try_to(encoder->open()); }
        return Code::OK;
    }

    Code Pipeline::openMediaSources() {
        for (auto&& processor : _processors) {
            auto source = dynamic_cast<MediaSource*>(processor);
            if (inited_ptr(source)) { try_to(source->open()); }
        }
        return Code::OK;
    }

    Code Pipeline::openMediaSinks() {
        for (auto&& processor : _processors) {
            auto sink = dynamic_cast<MediaSink*>(processor);
            if (inited_ptr(sink)) { try_to(sink->open()); }
        }
        return Code::OK;
    }

    Code Pipeline::closeMediaSources() {
        for (auto&& processor : _processors) {
            auto source = dynamic_cast<MediaSource*>(processor);
            if (inited_ptr(source)) { try_to(source->close()); }
        }
        return Code::OK;
    }

    Code Pipeline::closeMediaSinks() {
        for (auto&& processor : _processors) {
            auto sink = dynamic_cast<MediaSink*>(processor);
            if (inited_ptr(sink)) { try_to(sink->close()); }
        }
        return Code::OK;
    }

    Code Pipeline::startProcesors() {
        for (auto&& processor : _processors) {
            auto thread_processor = static_cast<Thread*>(processor);
            try_to(thread_processor->start());
        }
        return Code::OK;
    }

    Code Pipeline::stopProcesors() {
        for (auto&& processor : _processors) {
            auto thread_processor = static_cast<Thread*>(processor);
//            try_to(thread_processor->quit());
            log_error("Thread: " << thread_processor->name());
            try_to(thread_processor->stop());
        }
        return Code::OK;
    }

    Code Pipeline::joinProcesors() {
        for (auto&& processor : _processors) {
            auto thread_processor = static_cast<Thread*>(processor);
            thread_processor->join();
        }
        return Code::OK;
    }

//    Code Pipeline::determineSequences() {
//        StreamMap* stream_map = _map->streamMap();
//        return_if(stream_map->empty(), Code::NOT_INITED);
//        for (auto&& stream_route : *stream_map) {
//            Stream* in_stream = stream_route.first;
//            Stream* out_stream = stream_route.second;
//            StreamPair in_out_streams { in_stream, out_stream };
//            ProcessorSequence sequence;

//            sequence.push_back(static_cast<Processor*>(static_cast<FormatContext*>(in_stream->context())->_media_ptr));//TODO Крайне кривая заплатка
//            sequence.push_back(_map);

//            bool processing_required =
//                    (in_stream->isVideo() && !option(Option::COPY_VIDEO))
//                    || (in_stream->isAudio() && !option(Option::COPY_AUDIO)) ;

//            if (processing_required) {
//                bool rescaling_required     = utils::rescaling_required(in_out_streams);
//                bool resampling_required    = utils::resampling_required(in_out_streams);
//                bool video_filter_required  = utils::video_filter_required(in_out_streams);
//                bool audio_filter_required  = utils::audio_filter_required(in_out_streams);
//                bool transcoding_required   = utils::transcoding_required(in_out_streams);

//                auto out_context = dynamic_cast<OutputFormatContext*>(out_stream->context());
//                if (inited_ptr(out_context)) {
//                    video_filter_required = video_filter_required
//                                            || out_context->preset(IOType::Timelapse);
//                }

//                transcoding_required = (transcoding_required
//                                        || rescaling_required
//                                        || resampling_required);

////                bool decoding_required = transcoding_required
////                        && (in_stream->parameters->codecId() != AV_CODEC_ID_NONE);
////                bool encoding_required = transcoding_required
////                        && (out_stream->parameters->codecId() != AV_CODEC_ID_NONE);

//                bool decoding_required = transcoding_required //&& (in_stream->parameters->codecId() != AV_CODEC_ID_NONE))
//                                        || rescaling_required
//                                        || resampling_required
//                                        || video_filter_required
//                                        || audio_filter_required;
//                bool encoding_required = transcoding_required //&& (out_stream->parameters->codecId() != AV_CODEC_ID_NONE);
//                                        || rescaling_required
//                                        || resampling_required
//                                        || video_filter_required
//                                        || audio_filter_required;
//                if (inited_ptr(dynamic_cast<OpenCVSink*>(out_stream->context()))) {
//                    encoding_required = false;
//                }
////                encoding_required = encoding_required && inited_codec_id(out_stream->parameters->codecId());
//                //OpenCVSink

//                if (decoding_required) {
//                    Decoder* decoder = new Decoder(in_stream);
//                    sequence.push_back(decoder);
//                    addElement(decoder);
//                }

//                if (rescaling_required) {
//                    Rescaler* rescaler = new Rescaler(in_out_streams);
//                    sequence.push_back(rescaler);
//                    addElement(rescaler);
//                }

//                if (video_filter_required) {
////                    std::string filters_descr = "select='not(mod(n,10))',setpts=N/FRAME_RATE/TB";
////                    std::string filters_descr = "setpts=N/(10*TB)";
//                    std::string filters_descr = "select='not(mod(n,10))'";
//                    VideoFilter* video_filter = new VideoFilter(out_stream->parameters, filters_descr);
//                    sequence.push_back(video_filter);
//                    addElement(video_filter);
//                }

//                if (audio_filter_required) {
//                    //
//                }

//                if (resampling_required) {
//                    Resampler* resampler = new Resampler(in_out_streams);
//                    sequence.push_back(resampler);
//                    addElement(resampler);
//                }

//                if (encoding_required) {
//                    Encoder* encoder = new Encoder(out_stream);
//                    sequence.push_back(encoder);
//                    addElement(encoder);
//                }
//            }

//            auto ptr = dynamic_cast<FormatContext*>(out_stream->context());
//            if (inited_ptr(ptr)) {
//                sequence.push_back(static_cast<Processor*>(ptr->_media_ptr));
//            } else {
//                 sequence.push_back(static_cast<Processor*>(out_stream->context()));
//            }
////            sequence.push_back(static_cast<Processor*>(static_cast<FormatContext*>(out_stream->context())->_media_ptr));//TODO Крайне кривая заплатка
////            sequence.push_back(static_cast<Processor*>(out_stream->context()));

//            for (auto processor_it = sequence.begin(); processor_it != sequence.end(); processor_it++) {
//                auto next_processor_it = std::next(processor_it);
//                if (next_processor_it == sequence.end()) { break; }
//                log_debug((*processor_it)->name() + " connected to " + (*next_processor_it)->name());
//                if ((*processor_it)->is("MediaSource")) {
//                    try_to(static_cast<MediaSource*>(*processor_it)->connectTo(*next_processor_it));
//                } else if ((*processor_it)->is("Encoder")) {
//                    try_to(static_cast<Encoder*>(*processor_it)->connectTo(*next_processor_it));
//                } else if ((*processor_it)->is("Decoder")) {
//                    try_to(static_cast<Decoder*>(*processor_it)->connectTo(*next_processor_it));
//                } else if ((*processor_it)->is("Rescaler")) {
//                    try_to(static_cast<Rescaler*>(*processor_it)->connectTo(*next_processor_it));
//                } else if ((*processor_it)->is("Resampler")) {
//                    try_to(static_cast<Resampler*>(*processor_it)->connectTo(*next_processor_it));
//                } else if ((*processor_it)->is("VideoFilter")) {
//                    try_to(static_cast<VideoFilter*>(*processor_it)->connectTo(*next_processor_it));
//                } else {
//                    log_warning("didn't connected: " + (*processor_it)->name());
//                }
//            }

//            auto it = sequence.begin();
//            std::advance(it, 2);
//            auto first_packet_processor = *it;
////            auto debug_0 = dynamic_cast<PacketProcessor*>(first_packet_processor);
////            auto debug_1 = static_cast<PacketProcessor*>(first_packet_processor);
//            try_to(_map->setRoute(in_stream, dynamic_cast<Processor*>(first_packet_processor)));
//            _processor_sequences.push_back(sequence);
//        }

//        return Code::OK;
//    }

    void Pipeline::freeProcesors() //TODO объекты в смартпоинтеры
    {
        //см TODO ↗
//        for (auto&& proc : _processors) {
//            delete proc;
        //        }
    }

    Code Pipeline::createSequence(Route route) {
        Stream* input_stream = findStream(route.inputStreamUid());
        Stream* output_stream = findStream(route.outputStreamUid());
        StreamPair in_out_streams { input_stream, output_stream };

        output_stream->parameters->completeFrom(input_stream->parameters);
        input_stream->setUsed(true);
        output_stream->setUsed(true);

        try_to(route.append(static_cast<Processor*>(static_cast<FormatContext*>(input_stream->context())->_media_ptr)));

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
            try_to(addElement(decoder));
        }

        if (rescaling_required) {
            Rescaler* rescaler = new Rescaler(in_out_streams);
            try_to(route.append(rescaler));
            try_to(addElement(rescaler));
        }

        if (video_filter_required) {
//                    std::string filters_descr = "select='not(mod(n,10))',setpts=N/FRAME_RATE/TB";
//                    std::string filters_descr = "setpts=N/(10*TB)";
            std::string filters_descr = "select='not(mod(n,60))'";
            VideoFilter* video_filter = new VideoFilter(output_stream->parameters, filters_descr);
            try_to(route.append(video_filter));
            try_to(addElement(video_filter));
        }

        if (audio_filter_required) {
            return Code::NOT_IMPLEMENTED;
        }

        if (resampling_required) {
            Resampler* resampler = new Resampler(in_out_streams);
            try_to(route.append(resampler));
            try_to(addElement(resampler));
        }

        if (encoding_required) {
            Encoder* encoder = new Encoder(output_stream);
            try_to(route.append(encoder));
            try_to(addElement(encoder));
        }

        try_to(route.append(static_cast<Processor*>(static_cast<FormatContext*>(output_stream->context())->_media_ptr)));

        try_to(route.init());
        try_to(route.startAll());

        log_info("route: " << route);

        return Code::OK;
    }

    Stream* Pipeline::findStream(int64_t uid) {
        int64_t context_uid = utils::get_context_uid(uid);
        for (auto&& source : mediaSources()) {
            if (source->is("MediaSource")) {
                if (static_cast<MediaSource*>(source)->inputFormatContext().uid() == context_uid) {
                    for (auto&& stream : static_cast<MediaSource*>(source)->inputFormatContext().streams()) {
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
        for (auto&& sink : mediaSinks()) {
            if (sink->outputFormatContext().uid() == context_uid) {
                for (auto&& stream : sink->outputFormatContext().streams()) {
                    if (stream->uid() == uid) {
                        return stream;
                    }
                }
            }
        }
        return nullptr;
    }

    Code Pipeline::determineSequence(Object* output_processor) {
        StreamVector output_streams;
        if (output_processor->is("MediaSink")) {
            MediaSink* media_sink = static_cast<MediaSink*>(output_processor);
//            try_to(media_sink->init());
            output_streams = media_sink->outputFormatContext().streams();
            if (output_streams.empty()) {
                log_error("output_streams is empty: " << media_sink->outputFormatContext().mediaResourceLocator());
                return Code::NOT_INITED;
            }
            for (auto out_stream : output_streams) {
                Stream* in_stream = findBestInputStream(out_stream->type());
                if (not_inited_ptr(in_stream)) {
                    log_error("Failed to find input stream type " << out_stream->type());
                    return Code::INVALID_INPUT;
                }
                Route route;
                try_to(route.setMetaRoute(in_stream->uid(), out_stream->uid()));
                try_to(createSequence(route));
                _route_list.push_back(route);
            }
        }
        if (output_processor->is("CustomOutput")) {
            //
        }
    }

//    Code Pipeline::determineSequence(MediaSink* media_sink) {
//        try_to(connectIOStreams(MediaType::MEDIA_TYPE_VIDEO));
//        //TODO
//        return Code::OK;
//    }

    std::string Pipeline::toString() const {
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

        for (auto&& sink : mediaSinks()) {
            dump_str += "\n";
            dump_str += TAB;
            dump_str += sink->toString();
            for (auto i = 0; i < sink->outputFormatContext().numberStream(); i++) {
                dump_str += ":\n";
                dump_str += TAB;
                dump_str += TAB;
                dump_str += sink->outputFormatContext().toString(); //TODO tut
            }
        }

        /* Вывод информации о последовательностях обработки потоков
         * |  #1 Source[0:1] -> Decoder pcm_mulaw -> Resampler -> Encoder aac -> Sink[1:1]
         * |  #0 Source[0:0] -> Decoder flv -> Rescaler -> VideoFilter -> Encoder libx264 -> Sink[1:0]
        */
        dump_str += "\nStream mapping:";
        int64_t i = 0;
        std::string delimeter = " -> ";
//        auto stream_map = _map->streamMap();
        for (auto&& sequence : _processor_sequences) {
            dump_str += "\n";
            dump_str += TAB;
            dump_str += "#" + std::to_string(i++) + " ";
            for (auto&& elem : sequence) {
                if (elem->is("YMap")) { continue; }
                dump_str += elem->name();
                if (elem->is("Source") || elem->is("Sink")) { //TODO устаревший код
                    auto context = dynamic_cast<FormatContext*>(elem);
                    dump_str += "[" + std::to_string(context->uid())
                            + ":"
                            + std::to_string(i-1) + "]"; //TODO stream_index, брать из _stream_map->streamMap();
                }
                if (elem->is("Decoder")) {
                    dump_str += " " + std::string(static_cast<Decoder*>(elem)->decoderContext()->codec->name);
                }
                if (elem->is("Encoder")) {
                    dump_str += " " + std::string(static_cast<Encoder*>(elem)->encoderContext()->codec->name);
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
            for (auto&& source : mediaSources()) {
                auto medias_source = dynamic_cast<MediaSource*>(source);
                if (inited_ptr(medias_source)) {
                    all_video_streams.push_back(medias_source->inputFormatContext().bestStream(MediaType::MEDIA_TYPE_VIDEO));
                }
                auto custom_source = dynamic_cast<CustomPacketSource*>(source);
                if (inited_ptr(custom_source)) {
                    all_video_streams.push_back(custom_source->stream(MediaType::MEDIA_TYPE_VIDEO));
                }
            }
            return static_cast<VideoStream*>(utils::find_best_stream(all_video_streams));
        }
        case MediaType::MEDIA_TYPE_AUDIO: {
            StreamVector all_audio_streams;
            for (auto&& source : mediaSinks()) {
                all_audio_streams.push_back(source->outputFormatContext().bestStream(MediaType::MEDIA_TYPE_AUDIO));
            }
            return static_cast<AudioStream*>(utils::find_best_stream(all_audio_streams));
        }
        default:
            return nullptr;
        }
    }

    StreamList Pipeline::getOutputStreams(MediaType media_type) {
        StreamList output_streams;
        for (auto&& sink : mediaSinks()) {
            for (auto&& video_stream : sink->outputFormatContext().streams(media_type)) {
                output_streams.push_back(video_stream);
            }
        }
        for (auto&& sink : openCVSinks()) {
            output_streams.push_back(sink->videoStream());
        }
        return output_streams;
    }

    Route Pipeline::findRoute(Processor* processor) {
        for (auto&& route : _route_list) {
            if (route.contains(processor)) {
                return route;
            }
        }
        return Route();
    }

//    Code Pipeline::connectIOStreams(MediaType media_type) {
//        Stream* best_stream = findBestInputStream(media_type);
//        if (not_inited_ptr(best_stream)) {
//            log_warning(utils::media_type_to_string(media_type)
//                        << " stream is not present in the sources");

//            return Code::OK; //TODO ?? что возращать? //        return Code::INVALID_INPUT; ?
//        }
//        StreamList output_streams = getOutputStreams(media_type);
//        if (output_streams.empty()) {
//            log_warning("Sinks does not have any "
//                        << utils::media_type_to_string(media_type)
//                        << " stream");
//            log_info("Creating " << utils::media_type_to_string(media_type) << " stream in every sink...");
//            for (auto&& sink : mediaSinks()) {
//                if ((sink->outputFormatContext().preset() == IOType::Auto)
//                        && (sink->discardType(media_type) == false)) {
//                    try_to(sink->outputFormatContext().createStream(best_stream->parameters));
//                    log_info("Created "
//                             << utils::media_type_to_string(media_type)
//                             << " stream in "
//                             << sink->outputFormatContext().mediaResourceLocator());
//                }
//            }
//            output_streams = getOutputStreams(media_type);
//        }
//        if (output_streams.empty()) {
//            log_warning("Ignoring: " << *best_stream);
//        }
//        /* Трансляция наилучшего потока на все потоки выхода того же медиа-типа */
//        for (auto&& out_stream : output_streams) {
//            if (out_stream->used()) { continue; }
//            try_to(_map->addRoute(best_stream, out_stream));
//            out_stream->parameters->completeFrom(best_stream->parameters);
//            best_stream->setUsed(true);
//            out_stream->setUsed(true);
//        }
//        return Code::OK;
//    }

    MediaSourceList Pipeline::mediaSources() const {
        MediaSourceList source_list;
        for (auto&& processor : _processors) {
            auto source = dynamic_cast<MediaSource*>(processor);
            if (inited_ptr(source)) { source_list.push_back(source); }
            auto custom_source = dynamic_cast<CustomPacketSource*>(processor);
            if (inited_ptr(custom_source)) { source_list.push_back(custom_source); }
        }
        return source_list;
    }

    MediaSinkList Pipeline::mediaSinks() const {
        MediaSinkList sink_list;
        for (auto&& processor : _processors) {
            auto sink = dynamic_cast<MediaSink*>(processor);
            if (inited_ptr(sink)) { sink_list.push_back(sink); }
        }
        return sink_list;
    }

    OpenCVSinkList Pipeline::openCVSinks() const {
        OpenCVSinkList ocv_sink_list;
        for (auto&& processor : _processors) {
            auto sink = dynamic_cast<OpenCVSink*>(processor);
            if (inited_ptr(sink)) { ocv_sink_list.push_back(sink); }
        }
        return ocv_sink_list;
    }

    DecoderList Pipeline::decoders() const {
        DecoderList decoder_list;
        for (auto&& processor : _processors) {
            auto decoder = dynamic_cast<Decoder*>(processor);
            if (inited_ptr(decoder)) { decoder_list.push_back(decoder); }
        }
        return decoder_list;
    }

    EncoderList Pipeline::encoders() const {
        EncoderList encoder_list;
        for (auto&& processor : _processors) {
            auto encoder = dynamic_cast<Encoder*>(processor);
            if (inited_ptr(encoder)) { encoder_list.push_back(encoder); }
        }
        return encoder_list;
    }

//    Pipeline *createPipline()
//    {
//        return new Pipeline;
//    }

} // namespace fpp
