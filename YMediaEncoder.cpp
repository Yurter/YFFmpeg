#include "YMediaEncoder.h"
#include <iostream>

YMediaEncoder::YMediaEncoder(const YMediaDestination *destination)
{
    //
}

bool YMediaEncoder::init()
{
    std::cout << "[YMediaEncoder] Initializing" << std::endl;
    std::cout << "[YMediaEncoder] Initialized" << std::endl;
    return true;
}

AVPacket YMediaEncoder::encodeFrames(std::list<AVFrame> &&frames)
{
    AVPacket packet;
    return packet;
}

YMediaEncoder::~YMediaEncoder()
{
    //
}

bool YMediaEncoder::initVideoCodec()
{
    return false;
}

bool YMediaEncoder::initAudioCodec()
{
    return false;
}
