#pragma once

#include "base/YThread.h"
#include "context/YSource.h"
#include "codec/YVideoDecoder.h"
#include "refi/YVideoFilter.h"
#include "refi/YAudioFilter.h"
#include "codec/YVideoEncoder.h"
#include "context/YDestination.h"
#include "refi/YRescaler.h"
#include "refi/YResampler.h"
#include "YStreamMap.h"

class YMediaChain : public YThread
{

public:

    YMediaChain();
    ~YMediaChain() override;

    bool    init();
    bool    stop();
    void    pause();
    void    unpause();

    void    setOptions(int64_t options);
    void    addSource(const std::string& mrl, YMediaPreset preset = YMediaPreset::Auto);
    void    addDestination(const std::string& mrl, YMediaPreset preset = YMediaPreset::Auto);

private:

    YCode   run() override;

    bool    rescalerRequired();
    bool    resamplerRequired();

    bool contingencyVideoSourceRequired();
    bool contingencyAudioSourceRequired();

    bool    option(YOptions option);
    void parseOptions();

    void completeDestinationParametres();

    void    startProcesors();
    void    stopProcesors();
    void    freeProcesors();

private:

    // Media
    std::list<YThread*> _data_processors;

    // General
    volatile bool       _paused; 
    bool                _inited;

    int64_t             _options;

    int64_t             _source_video_stream_index;
    int64_t             _source_audio_stream_index;
    int64_t             _destination_video_stream_index;
    int64_t             _destination_audio_stream_index;

};
