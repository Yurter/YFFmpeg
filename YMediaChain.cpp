  #include "YMediaChain.h"

YMediaChain::YMediaChain(YMediaSource*      source,
                         YMediaDestination* destination,
                         int64_t            options) :
    YMediaChain(source,
                nullptr,
                nullptr,
                destination,
                options)
{
    //
}

YMediaChain::YMediaChain(YMediaSource*      source,
                         YMediaFilter*      video_filter,
                         YMediaFilter*      audio_filter,
                         YMediaDestination* destination,
                         int64_t            options) :
    _source(source),
    _decoder(new YMediaDecoder(source)),
    _video_filter(video_filter),
    _audio_filter(audio_filter),
    _encoder(new YMediaEncoder(destination)),
    _destination(destination),
    _rescaler(nullptr),
    _resampler(nullptr),
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

void YMediaChain::start()
{
    if (!init()) {
        std::cout << "[YMediaChain] Start failed" << std::endl;
        stop();
        return;
    }
    {
        _source_video_stream_index = _source->video_parameters.streamIndex();
        _source_audio_stream_index = _source->audio_parameters.streamIndex();
        _destination_video_stream_index = _destination->video_parameters.streamIndex();
        _destination_audio_stream_index = _destination->audio_parameters.streamIndex();
    }

    YThread::start();
    return;
}

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

void YMediaChain::setContingencyVideoSource(YMediaSource *contingency_video_source)
{
    _contingency_video_source = contingency_video_source;
}

void YMediaChain::setContingencyAudioSource(YMediaSource *contingency_audio_source)
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
        _rescaler = new YVideoRescaler();
        if (!_rescaler->init(_decoder->videoCodecContext()
                              , _encoder->videoCodecContext())) { return false; }
    }
    if (resamplerRequired()) {
        _resampler = new YAudioResampler();
        if (!_resampler->init(_decoder->audioCodecContext()
                              , _encoder->audioCodecContext())) { return false; }
    }

    _source->start();
    _decoder->start();
//    _rescaler->start();
    _resampler->start();
    _encoder->start();
    _destination->start();

    return true;
}

YCode YMediaChain::run()
{
    /*------------------------------- Чтение -------------------------------*/
    YPacket source_packet;
    if (!_source->pop(source_packet)) {
        if (_source->closed()) {
            return YCode::END_OF_FILE;
        }
        utils::sleep_for(10);
        return YCode::AGAIN;
    }
    if (skipPacket(source_packet)) { return YCode::AGAIN; }

    /*-------------------------- Сверка с опциями --------------------------*/
    bool process_packet = true;
    if (source_packet.isVideo() && optionInstalled(COPY_VIDEO)) {
        process_packet = false;
    }
    if (source_packet.isAudio() && optionInstalled(COPY_AUDIO)) {
        process_packet = false;
    }
    if (source_packet.isVideo()) {
        int deb = 0;
    }

    YPacket processed_packet;
    processed_packet.setType(source_packet.type()); //TODO refactoring <-

    if (process_packet) {
        /*---------------------------- Декодирование ---------------------------*/
        _decoder->pushPacket(source_packet);
        YFrame decoded_frame;
        if (!_decoder->popFrame(decoded_frame)) {
            return YCode::AGAIN;
        }

        //

        /*------------------------------ Рескейлинг ----------------------------*/
        // TODO

        /*------------------------------ Ресемплинг ----------------------------*/
        if (_resampler != nullptr) {
            if (decoded_frame.isAudio()) {
                _resampler->push(decoded_frame);
            }
        }
        YFrame resampled_frame;
        if (!_resampler->pop(resampled_frame)) {
            return YCode::AGAIN;
        }

        /*------------------------------ Кодирование ---------------------------*/
        _encoder->pushFrame(resampled_frame);
        if (!_encoder->popPacket(processed_packet)) {
            return YCode::AGAIN;
        }
        if (!mapStreamIndex(source_packet, processed_packet)) {
            std::cerr << "[YMediaChain] mapStreamIndex failed" << std::endl;
            return YCode::ERR;
        }
    } else { //TODO
        processed_packet = source_packet;
    }

    /*-------------------------------- Запись ------------------------------*/
    _destination->push(processed_packet);
    return YCode::OK;
}

bool YMediaChain::rescalerRequired()
{
    return false;
}

bool YMediaChain::resamplerRequired()
{
    auto& src = _source->audio_parameters;
    auto& dst = _destination->audio_parameters;
    if (!src.available()) {
        return false;
    }
    if (!dst.available()) {
        return false;
    }
    if (src.ignore()) {
        return false;
    }
    if (optionInstalled(COPY_AUDIO)) {
        return false;
    }
    if (src.sampleRate() != dst.sampleRate()) {
        return true;
    }
    if (src.sampleFormat() != dst.sampleFormat()) {
        return true;
    }
    if (src.chanels() != dst.chanels()) {
        return true;
    }
    if (src.chanelsLayout() != dst.chanelsLayout()) {
        return true;
    }
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

bool YMediaChain::mapStreamIndex(YPacket& src_packet, YPacket& dst_packet)
{
    if (src_packet.isVideo()) {
        dst_packet.raw().stream_index = static_cast<int>(_destination_video_stream_index);
        return true;
    }
    if (src_packet.isAudio()) {
        dst_packet.raw().stream_index = static_cast<int>(_destination_audio_stream_index);
        return true;
    }
    return false;
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
