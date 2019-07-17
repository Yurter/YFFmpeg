#include "YFFmpeg.hpp"

YFFmpeg::YFFmpeg() :
    _stream_map(new YMap)
{
    setName("YFFmpeg");
    _data_processors.push_back(_stream_map);
}

YFFmpeg::~YFFmpeg()
{
    freeProcesors();
    stop();
}

bool YFFmpeg::stop()
{
    stopProcesors();
    joinProcesors();
    log_info("Stopped");
    stop_log();
    return true;
}

void YFFmpeg::pause()
{
    _paused = true;
    log_info("Paused");
}

void YFFmpeg::unpause()
{
    log_info("Unpaused");
    _paused = false;
}

void YFFmpeg::setOptions(int64_t options)
{
    _options = options;
}

void YFFmpeg::addElement(YObject* element)
{
    _data_processors.push_back(element);
}

void YFFmpeg::setRoute(Route route)
{
    _stream_map->addRoute(route);
}

void YFFmpeg::setRoute(stream_context input_stream_context, stream_context output_stream_context)
{
    _stream_map->addRoute({ input_stream_context, output_stream_context });
}

void YFFmpeg::setRoute(YContext* input_context, int64_t input_stream_index, YContext* output_context, int64_t output_stream_index)
{
    _stream_map->addRoute({ { input_context, input_stream_index }
                            , { output_context, output_stream_index } });
}

void YFFmpeg::dump() const //TODO operator std::string()
{
    log_info(this);
}

YCode YFFmpeg::init()
{
    log_info("Initialization started...");
    try_to(initContext());
    try_to(openContext());
    try_to(determineSequences());
    try_to(initRefi());
    try_to(initCodec());
    try_to(startProcesors());
    dump();
    setInited(true);
    return YCode::OK;
}

YCode YFFmpeg::run()
{
    for (auto&& processor : _data_processors) {
        auto thread_processor = static_cast<YThread*>(processor);
        return_if_not(thread_processor->running()
                      , thread_processor->exitCode());
    }
    utils::sleep_for(LONG_DELAY_MS);
    return YCode::OK;
}

bool YFFmpeg::option(YOption option) const
{
    return _options & option;
}

YCode YFFmpeg::initMap()
{
    auto&& route_map = _stream_map->streamMap();
    if (route_map.empty()) {
        /* Пользователь не установил таблицу маршрутов явно,
         * определяются маршруты по умолчанию */
        /* Поиск наилучшего видео-потока */
        YVideoStream* best_video_stream = findBestVideoStream();
        std::list<YVideoStream*> destination_video_streams; ..TODO
        if (not_inited_ptr(best_video_stream) && !destination_video_streams.empty()) {
            log_warning("Destination requires a video stream that is not present in the source");
        }
        /* Трансляция наилучшего видео-потока на все видео-потоки выхода */
        for (auto&& out_video_stream : destination_video_streams) {
            _stream_map->addRoute(best_video_stream, out_video_stream);
        }
        /* Поиск наилучшего аудио-потока */
        YAudioStream* best_audio_stream = findBestAudioStream();
        std::list<YAudioStream*> destination_audio_streams;
        if (not_inited_ptr(best_audio_stream) && !destination_audio_streams.empty()) {
            log_warning("Destination requires a video stream that is not present in the source");
        }
        /* Трансляция наилучшего аудио-потока на все аудио-потоки выхода */
        for (auto&& out_audio_stream : destination_audio_streams) {
            _stream_map->addRoute(best_audio_stream, out_audio_stream);
        }
    } else {
        // испольозвать (только?) установленные маршруты, (!) изменения но не замена дефолтных
    }
    try_to(_stream_map->init());
    return YCode::OK;
}

YCode YFFmpeg::initRefi()
{
    for (auto&& processor : _data_processors) {
        auto refi = dynamic_cast<YRefi*>(processor);
        if (inited_ptr(refi)) { try_to(refi->init()); }
    }
    return YCode::OK;
}

YCode YFFmpeg::initCodec()
{
    for (auto&& processor : _data_processors) {
        auto decoder = dynamic_cast<YDecoder*>(processor);
        if (inited_ptr(decoder)) { try_to(decoder->init()); continue; }
        auto encoder = dynamic_cast<YEncoder*>(processor);
        if (inited_ptr(encoder)) { try_to(decoder->init()); continue; }
    }
    return YCode::OK;
}

YCode YFFmpeg::initContext()
{
    for (auto&& processor : _data_processors) {
        auto context = dynamic_cast<YContext*>(processor);
        if (inited_ptr(context)) { try_to(context->init()); }
    }
    return YCode::OK;
}

YCode YFFmpeg::openContext()
{
    for (auto&& processor : _data_processors) {
        auto context = dynamic_cast<YContext*>(processor);
        if (inited_ptr(context)) { try_to(context->open()); }
    }
    return YCode::OK;
}

YCode YFFmpeg::closeContext()
{
    for (auto&& processor : _data_processors) {
        auto context = dynamic_cast<YContext*>(processor);
        if (inited_ptr(context)) { try_to(context->close()); }
    }
    return YCode::OK;
}

YCode YFFmpeg::startProcesors()
{
    for (auto&& processor : _data_processors) {
        auto thread_processor = static_cast<YThread*>(processor);
        try_to(thread_processor->start());
    }
    return YCode::OK;
}

YCode YFFmpeg::stopProcesors()
{
    for (auto&& processor : _data_processors) {
        auto thread_processor = static_cast<YThread*>(processor);
        try_to(thread_processor->quit());
    }
    return YCode::OK;
}

YCode YFFmpeg::joinProcesors()
{
    for (auto&& processor : _data_processors) {
        auto thread_processor = static_cast<YThread*>(processor);
        thread_processor->join();
    }
    return YCode::OK;
}

YCode YFFmpeg::determineSequences() //TODO
{
    for (auto&& route : _stream_map->streamMap()) {
        YContext* input_context = route.first.first;
        YContext* output_context = route.second.first;
        return_if_not(input_context->inited(), YCode::NOT_INITED);
        return_if_not(output_context->inited(), YCode::NOT_INITED);
        YStream* input_stream = input_context->stream(route.first.second);
        YStream* output_stream = output_context->stream(route.second.second);
        return_if_not(input_stream->type() == output_stream->type(), YCode::INVALID_INPUT);
        return_if(input_context == output_context, YCode::INVALID_INPUT);
        std::list<YObject*> sequence;
        sequence.push_back(input_context);
        sequence.push_back(_stream_map);
        input_context->connectOutputTo(_stream_map);
        auto io_streams = StreamPair(input_stream, output_stream);
        if (input_stream->isVideo() && !option(YOption::COPY_VIDEO) && rescalerRequired(io_streams)) {
            //
            auto video_decoder = new YDecoder(input_stream);
            sequence.push_back(video_decoder);
            _data_processors_decoder.push_back(video_decoder);
            //
            auto rescaler = new YRescaler(io_streams);
            sequence.push_back(rescaler);
            _data_processors_refi.push_back(rescaler);
            video_decoder->connectOutputTo(rescaler);
            //
            auto video_encoder = new YEncoder(output_stream);
            sequence.push_back(video_encoder);
            _data_processors_encoder.push_back(video_encoder);
            rescaler->connectOutputTo(video_encoder);
            //
            sequence.push_back(output_context);
            video_encoder->connectOutputTo(output_context);
        } else {// TODO
            if (input_stream->isVideo()) {
                sequence.push_back(output_context);
            }
        }
        if (input_stream->isAudio() && !option(YOption::COPY_AUDIO) && resamplerRequired(io_streams)) {
            //
            auto audio_decoder = new YDecoder(input_stream);
            sequence.push_back(audio_decoder);
            _data_processors_decoder.push_back(audio_decoder);
            //
            auto resampler = new YResampler(io_streams);
            sequence.push_back(resampler);
            _data_processors_refi.push_back(resampler);
            audio_decoder->connectOutputTo(resampler);
            //
            auto audio_encoder = new YEncoder(output_stream);
            sequence.push_back(audio_encoder);
            _data_processors_encoder.push_back(audio_encoder);
            resampler->connectOutputTo(audio_encoder);
            //
            sequence.push_back(output_context);
            audio_encoder->connectOutputTo(output_context);
        } else {// TODO
            if (input_stream->isAudio()) {
                sequence.push_back(output_context);
            }
        }
        // TODO PUSH output_context
        auto first_proc_it = std::next(std::find(sequence.begin(), sequence.end(), _stream_map));
        auto debug_test = *first_proc_it;
        _stream_map->setRoute(input_stream, dynamic_cast<YAsyncQueue<YPacket>*>(*first_proc_it));
        _processor_sequences.push_back(sequence);
    }
    return_if_not(_stream_map->inited(), YCode::NOT_INITED);
    return YCode::OK;
}

void YFFmpeg::freeProcesors() //TODO объекты в смартпоинтеры
{
    //см TODO ↗
}

std::string YFFmpeg::toString() const
{
    //input сами в опене TODO
    //output сами в опене TODO
    //chains
//    for (auto&& context : _data_processors_context) { log_info(context->toString()); }


    std::string dump_str;

    /* Вывод информации о контекстах */
    /* Input #0, rtsp, from 'rtsp://admin:admin@192.168.10.3':
     *      Stream #0:0: Video: h264, 1920x1080 [16:9], 23 fps, 400 kb/s
     *      Stream #0:1: Audio: pcm_mulaw, 16000 Hz, 1 channels, s16, 128 kb/s
     * Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/ytub-8t5w-asjj-avyf':
     *      Stream #0:0: Video: h264, 1920x1080, 400 kb/s
     *      Stream #0:1: Audio: aac, 44100 Hz, stereo, 128 kb/s
    */
    for (auto&& context : _data_processors_context) {
        dump_str += "\n" + context->toString();
        for (auto i = 0; i < context->numberStream(); i++) {
            dump_str += "\n";
            dump_str += TAB + context->stream(i)->toString(); //TODO tut
        }
    }

    /* Вывод информации о последовательностях обработки потоков */ //TODO move code to YMap::toString() etc.
    dump_str += "\nProcessing sequences";
    int64_t i = 0;
    std::string delimeter = " -> ";
//    auto& stream_map = _stream_map->streamMap();
    //TODO
    // #1 YSource[0:1] -> YDecoder pcm_mulaw -> YResampler -> YEncoder aac -> YDestination[1:1]
    for (auto&& sequence : _processor_sequences) {
        dump_str += "\n";
        dump_str +=  TAB;
        dump_str += "#" + std::to_string(i++) + " ";
        for (auto&& elem : sequence) {
            if (elem->is("YMap")) { continue; }
            dump_str += elem->name();
            if (elem->is("YSource") || elem->is("YDestination")) {
                auto context = dynamic_cast<YContext*>(elem);
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

YVideoStream* YFFmpeg::findBestVideoStream()
{
    StreamVector all_video_streams;
    for (auto&& context : contexts()) {
        all_video_streams.push_back(context->bestVideoStream());
    }
    return static_cast<YVideoStream*>(utils::findBestVideoStream(all_video_streams));
}

YAudioStream* YFFmpeg::findBestAudioStream()
{
    StreamVector all_audio_streams;
    for (auto&& context : contexts()) {
        all_audio_streams.push_back(context->bestAudioStream());
    }
    return static_cast<YAudioStream*>(utils::findBestAudioStream(all_audio_streams));
}

ContextList YFFmpeg::contexts()
{
    ContextList context_list;
    for (auto&& processor : _data_processors) {
        auto context = dynamic_cast<YContext*>(processor);
        if (inited_ptr(context)) { context_list.push_back(context); }
    }
    return context_list;
}

DecoderList YFFmpeg::decoders()
{
    DecoderList decoder_list;
    for (auto&& processor : _data_processors) {
        auto decoder = dynamic_cast<YDecoder*>(processor);
        if (inited_ptr(decoder)) { decoder_list.push_back(decoder); }
    }
    return decoder_list;
}
