#include "YMediaChain.h"

YMediaChain::YMediaChain(YMediaSource*      source,
                         YMediaDestination* destination) :
    YMediaChain(source,
                nullptr,
                nullptr,
                destination)
{
    //
}

YMediaChain::YMediaChain(YMediaSource*      source,
                         YMediaFilter*      video_filter,
                         YMediaFilter*      audio_filter,
                         YMediaDestination* destination) :
    _source(source),
    _decoder(new YMediaDecoder(source)),
    _video_filter(video_filter),
    _audio_filter(audio_filter),
    _encoder(new YMediaEncoder(destination)),
    _destination(destination),
    _rescaler(nullptr),
    _resampler(nullptr),
    _active(false),
    _paused(false),
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
                        _destination->close(); //TODO: вылет при завершении main() - поток дест не завершен.
                        break;
                    }
                }
                if (skipPacket(&source_packet)) { continue; }

                /*---------------------------- Декодирование ---------------------------*/
                std::list<AVFrame*> decoded_frames;
                if (!_decoder->decodePacket(&source_packet, decoded_frames)) {
                    std::cerr << "[YMediaChain] Decode failed" << std::endl;
                    break;
                }
                if (decoded_frames.empty()) { continue; }

                /*------------------------------ Рескейлинг ----------------------------*/
                if (_rescaler != nullptr) {
                    if (isVideoPacket(&source_packet)) {
                        if (!_rescaler->rescale(decoded_frames.front())) {
                            std::cerr << "[YMediaChain] Rescale failed" << std::endl;
                            break;
                        }
                    }
                }

                /*------------------------------ Ресемплинг ----------------------------*/
                if (_resampler != nullptr) {
                    if (isAudioPacket(&source_packet)) {
                        if (!_resampler->resample(&decoded_frames.front())) {
                            std::cerr << "[YMediaChain] Resample failed" << std::endl;
                            break;
                        }
                    }
                }

                /*------------------------------ Кодирование ---------------------------*/
                AVPacket *destination_packet = av_packet_alloc();
                av_init_packet(destination_packet);
                if (!mapStreamIndex(&source_packet, destination_packet)) {
                    std::cerr << "[YMediaChain] mapStreamIndex failed" << std::endl;
                }
                if (!_encoder->encodeFrames(destination_packet, decoded_frames)) {
                    std::cerr << "[YMediaChain] Encode failed" << std::endl;
                    continue;
                }

                /*-------------------------------- Запись ------------------------------*/
                _destination->writePacket(*destination_packet);
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

bool YMediaChain::rescalerRequired()
{
    return false;
}

bool YMediaChain::resamplerRequired()
{
    auto& src = _source->audio_parameters;
    auto& dst = _destination->audio_parameters;
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

bool YMediaChain::isVideoPacket(AVPacket *packet)
{
    return (packet->stream_index == _source_video_stream_index);
}

bool YMediaChain::isAudioPacket(AVPacket *packet)
{
    return (packet->stream_index == _source_audio_stream_index);
}

bool YMediaChain::skipPacket(AVPacket *packet)
{
    bool skip_packet = (!_destination->video_parameters.available() && isVideoPacket(packet))
                    || (!_destination->audio_parameters.available() && isAudioPacket(packet));
    return skip_packet;
}

bool YMediaChain::mapStreamIndex(AVPacket *src_packet, AVPacket *dst_packet)
{
    if (isVideoPacket(src_packet)) {
        dst_packet->stream_index = static_cast<int>(_destination_video_stream_index);
        return true;
    }
    if (isAudioPacket(src_packet)) {
        dst_packet->stream_index = static_cast<int>(_destination_audio_stream_index);
        return true;
    }
    return false;
}
