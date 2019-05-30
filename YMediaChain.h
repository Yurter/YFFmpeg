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
                YMediaDestination*  destination,
                int64_t             options = 0);

    YMediaChain(YMediaSource*       source,
                YMediaFilter*       video_filter,
                YMediaFilter*       audio_filter,
                YMediaDestination*  destination,
                int64_t             options);

    ~YMediaChain();

    bool start();
    bool stop();
    void pause();
    void unpause();
    bool active();

//    void setContingencySource(YMediaSource* contingency_source);
    void setContingencyVideoSource(YMediaSource* contingency_video_source);
    void setContingencyAudioSource(YMediaSource* contingency_audio_source);

    //TODO
    void setVideoFilter(std::string video_filter);
    void setAudioFilter(std::string audio_filter);

private:

    bool init();

    bool rescalerRequired();
    bool resamplerRequired();

    bool contingencyVideoSourceRequired();
    bool contingencyAudioSourceRequired();

    bool skipPacket(AVPacket& packet);
    bool mapStreamIndex(AVPacket& src_packet, AVPacket& dst_packet);

    bool optionInstalled(YOptions option);
    void parseInstalledOptions();

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

    YMediaSource*       _contingency_video_source;
    YMediaSource*       _contingency_audio_source;

	//General parameters
	std::thread			_thread;
    volatile bool       _active;
    volatile bool       _paused; 

    int64_t             _options;

    int64_t             _source_video_stream_index;
    int64_t             _source_audio_stream_index;
    int64_t             _destination_video_stream_index;
    int64_t             _destination_audio_stream_index;

};
