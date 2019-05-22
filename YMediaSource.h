#pragma once

#include "YAbstractMedia.h"
#include "YMediaDestination.h"

class YMediaSource : public YAbstractMedia
{

public:

    YMediaSource(const std::string &mrl);   // mrl - media resource locator
    virtual ~YMediaSource();

    bool open();
    bool close();

    bool readPacket(AVPacket &packet);

private:

    bool openInput();
    void run();

private:

    // General parameters

};
