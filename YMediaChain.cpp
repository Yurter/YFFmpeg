#include "YMediaChain.h"

YMediaChain::YMediaChain(YMediaSource *source, YMediaDestination *destination) :
    YMediaChain(source, new YMediaFilter, destination)
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
    } else if (!_filter->init()) {
        start_failed = true;
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
                bool written = true;//_destination->writePacket(_encoder->encodeFrames(_filter->filterFrames(_decoder->decodePacket(_source->readPacket()))));
                if (!written) { stop(); }
            }
        });
        std::cout << "[YMediaChain] Started" << std::endl;
    }

    return true;
}

bool YMediaChain::stop()
{
    _active = false;
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
