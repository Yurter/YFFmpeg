#pragma once

#include "YMediaSource.h"
#include "YMediaDecoder.h"
#include "YMediaFilter.h"
#include "YMediaEncoder.h"
#include "YMediaDestination.h"
#include "YVideoRescaler.h"
#include "YAudioResampler.h"

#include <thread>
#include <iostream>

class YMediaChain
{

public:

    YMediaChain(YMediaSource*       source,
                YMediaDestination*  destination);

    YMediaChain(YMediaSource*       source,
                YMediaFilter*       video_filter,
                YMediaFilter*       audio_filter,
                YMediaDestination*  destination);

    ~YMediaChain();

    bool start();
    bool stop();
    void pause();
    void unpause();
    bool active();

private:

    bool rescalerRequired();
    bool resamplerRequired();

private:

	//YMedia
    YMediaSource*       _source;
    YMediaDecoder*      _decoder;
    YMediaFilter*       _video_filter;
    YMediaFilter*       _audio_filter;
    YMediaEncoder*      _encoder;
    YMediaDestination*  _destination;
    YVideoRescaler*     _rescaler;
    YAudioResampler*    _resampler;

	//General parameters
	std::thread			_thread;
    volatile bool       _active;
    volatile bool       _paused; 

};
