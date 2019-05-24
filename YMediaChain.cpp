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
    _paused(false)
{
    //
}

YMediaChain::~YMediaChain()
{
    //
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
        auto source_video_stream_index = _source->videoStreamIndex();
        auto source_audio_stream_index = _source->audioStreamIndex();
        _active = true;
        _thread = std::thread([this, source_video_stream_index, source_audio_stream_index](){
            while (_active) {
                AVPacket source_packet;
                if (!_source->readPacket(source_packet)) {
                    std::cerr << "[YMediaChain] No data available" << std::endl;
                    _destination->close(); //TODO: вылет при завершении main() - поток дест не завершен.
                    break;
                }

                std::list<AVFrame*> decoded_frames;
                if (!_decoder->decodePacket(&source_packet, decoded_frames)) {
                    std::cerr << "[YMediaChain] Decode failed" << std::endl;
                    break;
                }
                if (decoded_frames.empty()) { continue; }

                if (_rescaler != nullptr) {
                    if (source_packet.stream_index == source_video_stream_index) {
                        if (!_rescaler->rescale(decoded_frames.front())) {
                            std::cerr << "[YMediaChain] Rescale failed" << std::endl;
                            break;
                        }
                    }
                }

                if (_resampler != nullptr) {
                    if (source_packet.stream_index == source_audio_stream_index) {
                        if (!_resampler->resample(&decoded_frames.front())) {
                            std::cerr << "[YMediaChain] Resample failed" << std::endl;
                            break;
                        }
                    }
                }

                AVPacket *encoded_packet = av_packet_alloc();
                av_init_packet(encoded_packet);
                encoded_packet->stream_index = source_packet.stream_index;
                if (!_encoder->encodeFrames(encoded_packet, decoded_frames)) {
                    std::cerr << "[YMediaChain] Encode failed" << std::endl;
                    continue;
                }

                _destination->writePacket(*encoded_packet);
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
//    _thread.join();
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
    if (_source->sampleRate() != _destination->sampleRate()) {
        return true;
    }
    if (_source->sampleFormat() != _destination->sampleFormat()) {
        return true;
    }
    if (_source->audioChanels() != _destination->audioChanels()) {
        return true;
    }
    if (_source->audioChanelsLayout() != _destination->audioChanelsLayout()) {
        return true;
    }
    return false;
}
