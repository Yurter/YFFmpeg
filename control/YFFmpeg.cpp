#include "YFFmpeg.hpp"
#include <iterator>

YFFmpeg::YFFmpeg() :
    _map(new YMap)
{
    setName("YFFmpeg");
    _data_processors.push_back(_map);
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

//void YFFmpeg::setRoute(YStream* input_stream, YStream* output_stream)
//{
//    _map->addRoute(input_stream, output_stream);
//}

//TODO
void YFFmpeg::setRoute(YContext* input_context, int64_t input_stream_index
                       , YContext* output_context, int64_t output_stream_index) {
    _metamap.push_back({{ input_context->uid(), input_stream_index }
                    , { output_context->uid(), output_stream_index }});
}

void YFFmpeg::dump() const {
    log_info(toString());
}

YCode YFFmpeg::init()
{
    log_info("Initialization started...");
    try_to(checkIOContexts());
    try_to(initContext());
    try_to(openContext());
    try_to(initMap());
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

YCode YFFmpeg::checkIOContexts()
{
    return_if(sources().empty(),      YCode::NOT_INITED);   //TODO throw YException("No source specified");
    return_if(destinations().empty(), YCode::NOT_INITED);   //TODO throw YException("No destination specified");
    return YCode::OK;
}

YCode YFFmpeg::initMap()
{
    auto stream_map = _map->streamMap();
    if (stream_map->empty()) {
        /* Пользователь не установил таблицу маршрутов явно,
         * определяются маршруты по умолчанию */
        /* Проброс наилучшего видео-потока */
        try_to(connectIOStreams(YMediaType::MEDIA_TYPE_VIDEO));
        /* Проброс наилучшего аудио-потока */
        try_to(connectIOStreams(YMediaType::MEDIA_TYPE_AUDIO));
    }
    try_to(_map->init());
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
        if (inited_ptr(encoder)) { try_to(encoder->init()); continue; }
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

YCode YFFmpeg::determineSequences()
{
    StreamMap* stream_map = _map->streamMap();
    return_if(stream_map->empty(), YCode::NOT_INITED);
    for (auto&& stream_route : *stream_map) {
        YStream* in_stream = stream_route.first;
        YStream* out_stream = stream_route.second;
        ProcessorSequence sequence;

        sequence.push_back(in_stream->context());
        sequence.push_back(_map);

        bool rescaling_required   = utils::rescalingRequired({ in_stream, out_stream });
        bool resampling_required  = utils::resamplingRequired({ in_stream, out_stream });
        bool transcoding_required = utils::transcodingRequired({ in_stream, out_stream });

        transcoding_required = (transcoding_required
                                || rescaling_required
                                || resampling_required);

        if (transcoding_required) {
            YDecoder* decoder = new YDecoder(in_stream);
            sequence.push_back(decoder);
            addElement(decoder);
        }

        if (rescaling_required) {
            YRescaler* rescaler = new YRescaler({ in_stream, out_stream });
            sequence.push_back(rescaler);
            addElement(rescaler);
        }

        if (resampling_required) {
            YResampler* resampler = new YResampler({ in_stream, out_stream });
            sequence.push_back(resampler);
            addElement(resampler);
        }

        if (transcoding_required) {
            YEncoder* encoder = new YEncoder(out_stream);
            sequence.push_back(encoder);
            addElement(encoder);
        }

        sequence.push_back(out_stream->context());

        for (auto processor_it = sequence.begin(); processor_it != sequence.end(); processor_it++) {
            auto next_processor_it = std::next(processor_it);
            if (next_processor_it == sequence.end()) { break; }
//            log_error((*processor_it)->name() + " -> " + (*next_processor_it)->name());
            if ((*processor_it)->is("YSource")) {
                try_to(static_cast<YSource*>(*processor_it)->connectOutputTo(*next_processor_it));
//            } else if ((*processor_it)->is("YMap")) {
//                try_to(static_cast<YMap*>(*processor_it)->connectOutputTo(*next_processor_it));
            } else if ((*processor_it)->is("YEncoder")) {
                try_to(static_cast<YEncoder*>(*processor_it)->connectOutputTo(*next_processor_it));
            } else if ((*processor_it)->is("YDecoder")) {
                try_to(static_cast<YDecoder*>(*processor_it)->connectOutputTo(*next_processor_it));
            } else if ((*processor_it)->is("YRescaler")) {
                try_to(static_cast<YRescaler*>(*processor_it)->connectOutputTo(*next_processor_it));
            } else if ((*processor_it)->is("YResampler")) {
                try_to(static_cast<YResampler*>(*processor_it)->connectOutputTo(*next_processor_it));
            } else {
                log_error("FAIL " + (*processor_it)->name());
            }
        }

//        auto first_packet_processor = *(++++sequence.begin());
        auto it = sequence.begin();
        std::advance(it, 2);
        auto first_packet_processor = *it;
        _map->setRoute(in_stream, dynamic_cast<PacketProcessor*>(first_packet_processor));
        _processor_sequences.push_back(sequence);
    }

    return YCode::OK;
}

void YFFmpeg::freeProcesors() //TODO объекты в смартпоинтеры
{
    //см TODO ↗
}

std::string YFFmpeg::toString() const
{
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

YStream* YFFmpeg::findBestInputStream(YMediaType media_type)
{
    switch (media_type) {
    case YMediaType::MEDIA_TYPE_VIDEO: {
        StreamVector all_video_streams;
        for (auto&& source : sources()) {
            all_video_streams.push_back(source->bestStream(YMediaType::MEDIA_TYPE_VIDEO));
        }
        return static_cast<YVideoStream*>(utils::find_best_stream(all_video_streams));
    }
    case YMediaType::MEDIA_TYPE_AUDIO: {
        StreamVector all_audio_streams;
        for (auto&& source : sources()) {
            all_audio_streams.push_back(source->bestStream(YMediaType::MEDIA_TYPE_AUDIO));
        }
        return static_cast<YAudioStream*>(utils::find_best_stream(all_audio_streams));
    }
    default:
        return nullptr;
    }
}

StreamList YFFmpeg::getOutputStreams(YMediaType media_type)
{
    StreamList output_streams;
    for (auto&& destination : destinations()) {
        for (auto&& video_stream : destination->streams(media_type)) {
            output_streams.push_back(video_stream);
        }
    }
    return output_streams;
}

YCode YFFmpeg::connectIOStreams(YMediaType media_type)
{
    YStream* best_stream = findBestInputStream(media_type);
    if (not_inited_ptr(best_stream)) {
        log_warning("Destination requires a "
                    << utils::media_type_to_string(media_type)
                    << " stream that is not present in the source");
//        return YCode::INVALID_INPUT;
        return YCode::OK; //TODO ?? что возращать?
    }
    StreamList output_streams = getOutputStreams(media_type);
    /* Трансляция наилучшего потока на все потоки выхода того же медиа-типа */
    for (auto&& out_stream : output_streams) {
        try_to(_map->addRoute(best_stream, out_stream));
        best_stream->setUsed(true);
        out_stream->setUsed(true);
    }
    return YCode::OK;
}

ContextList YFFmpeg::contexts() const
{
    ContextList context_list;
    for (auto&& processor : _data_processors) {
        auto context = dynamic_cast<YContext*>(processor);
        if (inited_ptr(context)) { context_list.push_back(context); }
    }
    return context_list;
}

SourceList YFFmpeg::sources() const
{
    SourceList source_list;
    for (auto&& processor : _data_processors) {
        auto source = dynamic_cast<YSource*>(processor);
        if (inited_ptr(source)) { source_list.push_back(source); }
    }
    return source_list;
}

DestinationList YFFmpeg::destinations() const
{
    DestinationList destination_list;
    for (auto&& processor : _data_processors) {
        auto destination = dynamic_cast<YDestination*>(processor);
        if (inited_ptr(destination)) { destination_list.push_back(destination); }
    }
    return destination_list;
}

DecoderList YFFmpeg::decoders() const
{
    DecoderList decoder_list;
    for (auto&& processor : _data_processors) {
        auto decoder = dynamic_cast<YDecoder*>(processor);
        if (inited_ptr(decoder)) { decoder_list.push_back(decoder); }
    }
    return decoder_list;
}
