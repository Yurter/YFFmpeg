#pragma once

#include "base/YThread.h"
#include "YSource.h"
#include "YDecoder.h"
#include "YVideoFilter.h"
#include "YAudioFilter.h"
#include "YEncoder.h"
#include "YDestination.h"
#include "YRescaler.h"
#include "YResampler.h"
#include "YStreamMap.h"

#include <iostream>

class YMediaChain : public YThread
{

public:

    YMediaChain(YSource*       source,
                YDestination*  destination,
                int64_t             options = 0);

    YMediaChain(YSource*       source,
                YVideoFilter*  video_filter,
                YAudioFilter*  audio_filter,
                YDestination*  destination,
                int64_t        options);

    ~YMediaChain() override;

//    void start() override;
    bool stop();
    void pause();
    void unpause();
    bool active();

//    void setContingencySource(YSource* contingency_source);
    void setContingencyVideoSource(YSource* contingency_video_source);
    void setContingencyAudioSource(YSource* contingency_audio_source);

    //TODO
    void setVideoFilter(std::string video_filter);
    void setAudioFilter(std::string audio_filter);


    bool init();
private:
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
    YSource*            _source;
    YDecoder*           _decoder;
    YRescaler*          _rescaler;
    YResampler*         _resampler;
    YVideoFilter*       _video_filter;
    YAudioFilter*       _audio_filter;
    YEncoder*           _encoder;
    YStreamMap*         _stream_map;
    YDestination*       _destination;

    YSource*       _contingency_video_source;
    YSource*       _contingency_audio_source;

    // General
    volatile bool       _paused; 

    int64_t             _options;

    int64_t             _source_video_stream_index;
    int64_t             _source_audio_stream_index;
    int64_t             _destination_video_stream_index;
    int64_t             _destination_audio_stream_index;

};
