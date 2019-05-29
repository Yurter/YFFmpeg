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
    _active(false),
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

bool YMediaChain::start()
{
    if (_active) {
        std::cout << "[YMediaChain] Already started" << std::endl;
        return false;
    } else {
        std::cout << "[YMediaChain] Initialization started..." << std::endl;
    }

    bool start_failed = false;

    if (!_source->open()) {
        start_failed = true;
    } else if (!_decoder->init()) {
        start_failed = true;
    } else if (!_encoder->init()) {
        start_failed = true;
    } else if (!_destination->open()) {
        start_failed = true;
    }

    if (contingencyAudioSourceRequired()) {
        std::cout << "[YMediaChain] contingencyAudioSourceRequired" << std::endl;
        if (!_contingency_audio_source->open()) {
            start_failed = true;
        }
    }

    if (rescalerRequired()) {
        _rescaler = new YVideoRescaler();
        if (!_rescaler->init(_decoder->videoCodecContext()
                              , _encoder->videoCodecContext())) {
            start_failed = true;
        }
    }

    if (resamplerRequired()) {
        _resampler = new YAudioResampler();
        if (!_resampler->init(_decoder->audioCodecContext()
                              , _encoder->audioCodecContext())) {
            start_failed = true;
        }
    }

    if (start_failed) {
        std::cout << "[YMediaChain] Start failed" << std::endl;
        stop();
        return false;
    }

    {
        _source_video_stream_index = _source->video_parameters.streamIndex();
        _source_audio_stream_index = _source->audio_parameters.streamIndex();
        _destination_video_stream_index = _destination->video_parameters.streamIndex();
        _destination_audio_stream_index = _destination->audio_parameters.streamIndex();

        _active = true;
        _thread = std::thread([this]() {
            while (_active) {
                /*------------------------------- Чтение -------------------------------*/
                AVPacket source_packet;
                if (!_source->readPacket(source_packet)) {
                    std::cerr << "[YMediaChain] No data available" << std::endl;
                    if (!_source->opened()) {
                        _destination->close();
                        break;
                    }
                }
                if (skipPacket(source_packet)) { continue; }

                /*-------------------------- Сверка с опциями --------------------------*/
                bool process_packet = true;
                if (_source->isVideoPacket(source_packet) && optionInstalled(COPY_VIDEO)) {
                    process_packet = false;
                }
                if (_source->isAudioPacket(source_packet) && optionInstalled(COPY_AUDIO)) {
                    process_packet = false;
                }

                AVPacket processed_packet;

                if (process_packet) {
                    /*---------------------------- Декодирование ---------------------------*/
                    std::list<AVFrame*> decoded_frames;
                    if (!_decoder->decodePacket(&source_packet, decoded_frames)) {
                        std::cerr << "[YMediaChain] Decode failed" << std::endl;
                        break;
                    }
                    if (decoded_frames.empty()) { continue; }

                    /*------------------------------ Рескейлинг ----------------------------*/
                    if (_rescaler != nullptr) {
                        if (_source->isVideoPacket(source_packet)) {
                            if (!_rescaler->rescale(decoded_frames.front())) {
                                std::cerr << "[YMediaChain] Rescale failed" << std::endl;
                                break;
                            }
                        }
                    }

                    /*------------------------------ Ресемплинг ----------------------------*/
                    if (_resampler != nullptr) {
                        if (_source->isAudioPacket(source_packet)) {
                            if (!_resampler->resample(&decoded_frames.front())) {
                                std::cerr << "[YMediaChain] Resample failed" << std::endl;
                                break;
                            }
                        }
                    }

                    /*------------------------------ Кодирование ---------------------------*/
                    av_init_packet(&processed_packet);
                    if (!mapStreamIndex(source_packet, processed_packet)) {
                        std::cerr << "[YMediaChain] mapStreamIndex failed" << std::endl;
                    }
                    if (!_encoder->encodeFrames(&processed_packet, decoded_frames)) {
                        std::cerr << "[YMediaChain] Encode failed" << std::endl;
                        continue;
                    }
                } else {
                    processed_packet = source_packet;
                }

                /*-------------------------------- Запись ------------------------------*/
                _destination->writePacket(processed_packet);

                /*--------------------------------- TODO -------------------------------*/
//                if (contingencyAudioSourceRequired()) {
//                    if (_destination->_video_packet_index % (int)_destination->video_parameters.frameRate() != 0) { continue; }
//                    AVPacket silence_packet;
//                    if (!_contingency_audio_source->readPacket(silence_packet)) {
//                        std::cerr << "[YMediaChain] _contingency_audio_source No data available" << std::endl;
//                        if (!_contingency_audio_source->opened()) {
//                            break;
//                        }
//                    }
//                    silence_packet.stream_index = static_cast<int>(_destination_audio_stream_index);
//                    _destination->writePacket(silence_packet);
//                }
            }
            _active = false;
            std::cout << "[YMediaChain] Finished" << std::endl;
        });
        std::cout << "[YMediaChain] Started" << std::endl;
    }

    return true;
}

bool YMediaChain::stop()
{
    _active = false;
    _source->close();
    _destination->close();
    _thread.join();
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
    return _active;
}

void YMediaChain::setContingencyVideoSource(YMediaSource *contingency_video_source)
{
    _contingency_video_source = contingency_video_source;
}

void YMediaChain::setContingencyAudioSource(YMediaSource *contingency_audio_source)
{
    _contingency_audio_source = contingency_audio_source;
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

bool YMediaChain::skipPacket(AVPacket &packet)
{
    bool skip_packet = (!_destination->video_parameters.available() && _source->isVideoPacket(packet))
                    || (!_destination->audio_parameters.available() && _source->isAudioPacket(packet));
    return skip_packet;
}

bool YMediaChain::mapStreamIndex(AVPacket& src_packet, AVPacket& dst_packet)
{
    if (_source->isVideoPacket(src_packet)) {
        dst_packet.stream_index = static_cast<int>(_destination_video_stream_index);
        return true;
    }
    if (_source->isAudioPacket(src_packet)) {
        dst_packet.stream_index = static_cast<int>(_destination_audio_stream_index);
        return true;
    }
    return false;
}

bool YMediaChain::optionInstalled(YOptions option)
{
    return _options & option;
}
