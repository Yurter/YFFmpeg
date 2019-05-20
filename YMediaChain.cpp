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
    _resampler(new YAudioResampler()),
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

//    if (!_video_filter->init(_source, _decoder)) {
//        start_failed = true;
//    } else if (!_audio_filter->init(_source, _decoder)) {
//        start_failed = true;
//    }
    if (!_resampler->init(_decoder->audioCodecContext(), _encoder->audioCodecContext())) {
        start_failed = true;
    }


    if (start_failed) {
        std::cout << "[YMediaChain] Start failed" << std::endl;
        stop();
        return false;
    }

    {
        _active = true;
        _thread = std::thread([this](){
            while (_active) {
                AVPacket source_packet;
                if (!_source->readPacket(source_packet)) {
                    std::cerr << "[YMediaChain] Read failed" << std::endl;
                    break;
                }
//                if (source_packet.stream_index == 1) { continue; }
                std::list<AVFrame*> decoded_frames;
                if (!_decoder->decodePacket(&source_packet, decoded_frames)) {
                    std::cerr << "[YMediaChain] Decode failed" << std::endl;
                    break;
                }

                if (decoded_frames.empty()) { continue; }

                /*-------------------------filters-------------------------*/
//                if (source_packet.stream_index == AVMEDIA_TYPE_VIDEO) {
//                    if (!_video_filter->filterFrames(decoded_frames)) {
//                        std::cerr << "[YMediaChain] Filter failed" << std::endl;
//                        break;
//                    }
//                }
//                if (source_packet.stream_index == AVMEDIA_TYPE_AUDIO) {
//                    if (!_audio_filter->filterFrames(decoded_frames)) {
//                        std::cerr << "[YMediaChain] Filter failed" << std::endl;
//                        break;
//                    }
//                }
                /*----------------------------------------------------------*/

//                if (source_packet.stream_index == AVMEDIA_TYPE_AUDIO) {
//                    if (!_resampler->resample(decoded_frames.front())) {
//                        std::cerr << "[YMediaChain] Resaple failed" << std::endl;
//                        break;
//                    }
//                }

                AVPacket *encoded_packet = av_packet_alloc();
                av_init_packet(encoded_packet);
                encoded_packet->stream_index = source_packet.stream_index;
                if (!_encoder->encodeFrames(encoded_packet, decoded_frames)) {
                    std::cerr << "[YMediaChain] Encode failed" << std::endl;
                    continue;
//                    break;
                }
                if (!_destination->writePacket(*encoded_packet)) {
                    std::cerr << "[YMediaChain] Write failed" << std::endl;
                    break;
                }
//                if (!_destination->writePacket(source_packet)) {
//                    std::cerr << "[YMediaChain] Write failed" << std::endl;
//                    break;
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
