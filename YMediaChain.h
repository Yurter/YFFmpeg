#pragma once

#include "base/YThread.h"
#include "YMediaSource.h"
#include "YMediaDecoder.h"
#include "YMediaFilter.h"
#include "YMediaEncoder.h"
#include "YMediaDestination.h"
#include "YVideoRescaler.h"
#include "YAudioResampler.h"
#include "YStreamMap.h"

#include <thread>
#include <iostream>

class YMediaChain : public YThread
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

    ~YMediaChain() override;

    void start() override;
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
    YCode run() override;

    bool rescalerRequired();
    bool resamplerRequired();

    bool contingencyVideoSourceRequired();
    bool contingencyAudioSourceRequired();

    bool skipPacket(YPacket& packet);
    bool mapStreamIndex(YPacket& packet, YMediaType type);

    bool optionInstalled(YOptions option);
    void parseInstalledOptions();

    void completeDestinationParametres();

private:

    // Media
    YMediaSource*       _source;
    YMediaDecoder*      _decoder;
    YMediaFilter*       _video_filter;
    YMediaFilter*       _audio_filter;
    YMediaEncoder*      _encoder;
    YMediaDestination*  _destination;
    YVideoRescaler*     _rescaler;
    YAudioResampler*    _resampler;
    YStreamMap*         _stream_map;

    YMediaSource*       _contingency_video_source;
    YMediaSource*       _contingency_audio_source;

    // General
    volatile bool       _paused; 

    int64_t             _options;

    int64_t             _source_video_stream_index;
    int64_t             _source_audio_stream_index;
    int64_t             _destination_video_stream_index;
    int64_t             _destination_audio_stream_index;

};
