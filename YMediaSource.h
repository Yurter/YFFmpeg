#pragma once

#include "YAbstractMedia.h"
#include "YMediaDestination.h"
#include <list>
#include <thread>

class YMediaSource : public YAbstractMedia
{
public:

    YMediaSource(const std::string &mrl);   // mrl - media resource locator
    virtual ~YMediaSource();

    bool open();
    bool close();

    AVPacket readPacket();

private:

	// General parameters
    //
};
