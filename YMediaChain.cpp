#include "YMediaChain.h"

YMediaChain::YMediaChain(YMediaSource *source, YMediaDestination *destination) :
    YMediaChain(source, new YMediaFilter(""), destination)
{
    //
}

YMediaChain::YMediaChain(YMediaSource *source,
                         YMediaFilter *filter,
                         YMediaDestination *destination) :
    _source(source),
    _decoder(new YMediaDecoder(source)),
    _filter(filter),
    _encoder(new YMediaEncoder(destination)),
    _destination(destination),
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
//    } else if (!_filter->init(_source, _decoder)) {
//        start_failed = true;
    } else if (!_encoder->init()) {
        start_failed = true;
    } else if (!_destination->open()) {
        start_failed = true;
    }

    if (start_failed) {
        _source->close();
        _destination->close();
        std::cout << "[YMediaChain] Start failed" << std::endl;
        return false;
    }

    {
        _active = true;
        _thread = std::thread([this](){
            while (_active) {
                AVPacket source_packet;
                if (!_source->readPacket(source_packet)) {
                    std::cerr << "[YMediaChain] Read failed" << std::endl;
                    continue;
                    break;
                }
                std::list<AVFrame*> decoded_frames;
                if (!_decoder->decodePacket(&source_packet, decoded_frames)) {
                    std::cerr << "[YMediaChain] Decode failed" << std::endl;
                    break;
                }
                if (source_packet.stream_index == AVMEDIA_TYPE_VIDEO) {
                    if (!_filter->filterFrames(decoded_frames)) {
                        std::cerr << "[YMediaChain] Filter failed" << std::endl;
                        break;
                    }
                }
                if (source_packet.stream_index == AVMEDIA_TYPE_AUDIO) {
                    if (!_filter->filterFrames(decoded_frames)) {
                        std::cerr << "[YMediaChain] Filter failed" << std::endl;
                        break;
                    }
                }
                AVPacket *encoded_packet = av_packet_alloc();
                encoded_packet->stream_index = source_packet.stream_index;
                if (!_encoder->encodeFrames(encoded_packet, decoded_frames)) {
                    std::cerr << "[YMediaChain] Encode failed" << std::endl;
                    continue;
                }
                if (!_destination->writePacket(source_packet)) {
                    std::cerr << "[YMediaChain] Write failed" << std::endl;
                    break;
                }
                std::cout << "[YMediaChain] LOOP" << std::endl;
            }
            stop();
        });
        std::cout << "[YMediaChain] Started" << std::endl;
    }

    return true;
}

bool YMediaChain::stop()
{
    _active = false;
    _thread.join();
//    if (_thread.joinable()) { _thread.join(); }
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
