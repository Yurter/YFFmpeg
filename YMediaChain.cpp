  #include "YMediaChain.h"

YMediaChain::YMediaChain(YSource*      source,
                         YDestination* destination,
                         int64_t            options) :
    YMediaChain(source,
                nullptr,
                nullptr,
                destination,
                options)
{
    //
}

YMediaChain::YMediaChain(YSource*      source,
                         YVideoFilter* video_filter,
                         YAudioFilter* audio_filter,
                         YDestination* destination,
                         int64_t       options) :
    _source(source),
    _decoder(new YDecoder(source)),
    _rescaler(nullptr),
    _resampler(nullptr),
    _video_filter(video_filter),
    _audio_filter(audio_filter),
    _encoder(new YEncoder(destination)),
    _destination(destination),
    _contingency_video_source(nullptr),
    _contingency_audio_source(nullptr),
    _paused(false),
    _options(options),
    _source_video_stream_index(-1),
    _source_audio_stream_index(-1),
    _destination_video_stream_index(-1),
    _destination_audio_stream_index(-1)
{
    //
}

YMediaChain::~YMediaChain()
{
    stop();
}

//void YMediaChain::start()
//{
//    if (!init()) {
//        std::cout << "[YMediaChain] Start failed" << std::endl;
//        stop();
//        return;
//    }
//    {
//        _source_video_stream_index = _source->video_parameters.streamIndex();
//        _source_audio_stream_index = _source->audio_parameters.streamIndex();
//        _destination_video_stream_index = _destination->video_parameters.streamIndex();
//        _destination_audio_stream_index = _destination->audio_parameters.streamIndex();
//    }

//    YThread::start();
//    return;
//}

bool YMediaChain::stop()
{
    _source->close();
    _destination->close();
    std::cout << "[YMediaChain] Stopped" << std::endl;
    return true;
}

void YMediaChain::pause()
{
    _paused = true;
}

void YMediaChain::unpause()
{
    _paused = false;
}

bool YMediaChain::active()
{
    return running();
}

void YMediaChain::setContingencyVideoSource(YSource *contingency_video_source)
{
    _contingency_video_source = contingency_video_source;
}

void YMediaChain::setContingencyAudioSource(YSource *contingency_audio_source)
{
    _contingency_audio_source = contingency_audio_source;
}

void YMediaChain::setVideoFilter(std::string video_filter)
{
    video_filter = ""; //TODO
}

void YMediaChain::setAudioFilter(std::string audio_filter)
{
    audio_filter = ""; //TODO
}

bool YMediaChain::init()
{
    std::cout << "[YMediaChain] Initialization started..." << std::endl;

    if (!_source->open())       { return false; }

    parseInstalledOptions();
    completeDestinationParametres();

    if (!_decoder->init())      { return false; }
    if (!_encoder->init())      { return false; }
    if (!_destination->open())  { return false; }

    if (contingencyVideoSourceRequired()) {
        std::cout << "[YMediaChain] Contingency video source required" << std::endl;
        if (!_contingency_video_source->open()) { return false; }
    }
    if (contingencyAudioSourceRequired()) {
        std::cout << "[YMediaChain] Contingency audio source required" << std::endl;
        if (!_contingency_audio_source->open()) { return false; }
    }
    if (rescalerRequired()) {
        _rescaler = new YRescaler();
        if (!_rescaler->init(_decoder->videoCodecContext()
                              , _encoder->videoCodecContext())) { return false; }
    }
    if (resamplerRequired()) {
        _resampler = new YResampler();
        if (!_resampler->init(_decoder->audioCodecContext()
                              , _encoder->audioCodecContext())) { return false; }
        _resampler->setIgnoreType(YMediaType::MEDIA_TYPE_VIDEO); //TODO
    }

    //TODO
    YStream* inVidStr = _source->stream(_source->video_parameters.streamIndex());
    YStream* inAudStr = _source->stream(_source->video_parameters.streamIndex());
    YStream* ouVidStr = _destination->stream(_destination->video_parameters.streamIndex());
    YStream* ouAudStr = _destination->stream(_destination->video_parameters.streamIndex());
//    _stream_map->addRoute(inVidStr, ouVidStr);
//    _stream_map->addRoute(inAudStr, ouAudStr);
    //

    if (optionInstalled(COPY_VIDEO)) {
        _decoder->setSkipType(YMediaType::MEDIA_TYPE_VIDEO);
        _rescaler->setSkipType(YMediaType::MEDIA_TYPE_VIDEO);
//        _video_filter->setSkipType(YMediaType::MEDIA_TYPE_VIDEO);
    }

    if (optionInstalled(COPY_AUDIO)) {
        _decoder->setSkipType(YMediaType::MEDIA_TYPE_AUDIO);
        _resampler->setSkipType(YMediaType::MEDIA_TYPE_AUDIO);
//        _audio_filter->setSkipType(YMediaType::MEDIA_TYPE_AUDIO);
    }

    _source->connectOutputTo(_decoder);
    _decoder->connectOutputTo(_resampler);
    _resampler->connectOutputTo(_encoder);
    _encoder->connectOutputTo(_stream_map);
    _stream_map->connectOutputTo(_destination);

    _source->start();
    _decoder->start();
    _resampler->start();
    _resampler->setIgnoreType(YMediaType::MEDIA_TYPE_VIDEO); //TODO!!!
    _encoder->start();
    _stream_map->start();
    _destination->start();

    return true;
}

YCode YMediaChain::run()
{
//    if (!_inited) //TODO
    if (_source->running() == false)        { return YCode::ERR; } //TODO return lastError
    if (_decoder->running() == false)       { return YCode::ERR; }
    if (_resampler->running() == false)     { return YCode::ERR; }
    if (_encoder->running() == false)       { return YCode::ERR; }
    if (_destination->running() == false)   { return YCode::ERR; }

    utils::sleep_for(LONG_DELAY_MS);
    return YCode::OK;
}

bool YMediaChain::rescalerRequired()
{
    return true;
//    return false;
}

bool YMediaChain::resamplerRequired()
{
    auto& src = _source->audio_parameters;
    auto& dst = _destination->audio_parameters;
    if (!src.available())               { return false; }
    if (!dst.available())               { return false; }
    if (src.ignore())                   { return false; }
    if (optionInstalled(COPY_AUDIO))    { return false; }
    if (src.sampleRate() != dst.sampleRate())       { return true; }
    if (src.sampleFormat() != dst.sampleFormat())   { return true; }
    if (src.chanels() != dst.chanels())             { return true; }
    if (src.chanelsLayout() != dst.chanelsLayout()) { return true; }
    return false;
}

bool YMediaChain::contingencyVideoSourceRequired()
{
    return false;
}

bool YMediaChain::contingencyAudioSourceRequired()
{
    return !_source->audio_parameters.available()
            || _source->audio_parameters.ignore();
}

bool YMediaChain::skipPacket(YPacket &packet)
{
    bool skip_packet = (!_destination->video_parameters.available() && packet.isVideo())
                    || (!_destination->audio_parameters.available() && packet.isAudio());
    return skip_packet;
}

bool YMediaChain::mapStreamIndex(YPacket &packet, YMediaType type)
{
    switch (type) {
    case YMediaType::MEDIA_TYPE_VIDEO:
        packet.raw().stream_index = static_cast<int>(_destination_video_stream_index);
        return true;
    case YMediaType::MEDIA_TYPE_AUDIO:
        packet.raw().stream_index = static_cast<int>(_destination_audio_stream_index);
        return true;
    default:
        return false;
    }
}
bool YMediaChain::optionInstalled(YOptions option)
{
    return _options & option;
}

void YMediaChain::parseInstalledOptions()
{
    if (optionInstalled(COPY_VIDEO)) {
        _destination->video_parameters = _source->video_parameters;
    }
    if (optionInstalled(COPY_AUDIO)) {
        _destination->audio_parameters = _source->audio_parameters;
    }
}

void YMediaChain::completeDestinationParametres()
{
    _destination->video_parameters.softCopy(_source->video_parameters);
    _destination->audio_parameters.softCopy(_source->audio_parameters);
}
