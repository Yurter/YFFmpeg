#pragma once

#include "YMediaSource.h"
#include "YMediaDecoder.h"
#include "YMediaFilter.h"
#include "YMediaEncoder.h"
#include "YMediaDestination.h"

#include <thread>
#include <iostream>

class YMediaChain
{

public:

    YMediaChain(YMediaSource		*source,
                YMediaDestination	*destination);

    YMediaChain(YMediaSource		*source,
                YMediaFilter		*filter,
                YMediaDestination	*destination);

    ~YMediaChain();

    bool start();
    bool stop();
    void pause();
    void unpause();
    bool active();

private:

	//YMedia
	YMediaSource		*_source;
	YMediaDecoder		*_decoder;
	YMediaFilter		*_filter;
	YMediaEncoder		*_encoder;
	YMediaDestination	*_destination;

	//General parameters
	std::thread			_thread;
    volatile bool       _active;
    volatile bool       _paused;

};
