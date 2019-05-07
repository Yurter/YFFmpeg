#pragma once

#include "YAbstractMedia.h"
#include "YMediaDestination.h"
#include <queue>
#include <thread>
#include <mutex>

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
    void startRead();
    void queuePacket(AVPacket packet);

private:

	// General parameters
    std::queue<AVPacket>        _packet_queue;
    std::mutex                  _packet_queue_mutex;

};
