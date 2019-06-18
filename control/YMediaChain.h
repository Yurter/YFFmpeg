#pragma once

#include "base/YThread.h"
#include "context/YSource.h"
#include "context/YDestination.h"
#include "codec/YDecoder.h"
#include "codec/YEncoder.h"
#include "refi/YRescaler.h"
#include "refi/YResampler.h"
#include "refi/YVideoFilter.h"
#include "refi/YAudioFilter.h"
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
    void    addElement(YObject* element);

private:

    YCode   run() override;

    bool    rescalerRequired();
    bool    resamplerRequired();

    bool contingencyVideoSourceRequired();
    bool contingencyAudioSourceRequired();

    bool    option(YOptions option);
    void parseOptions();

    void completeDestinationParametres();

    YCode   startProcesors();
    YCode   stopProcesors();
    YCode   openProcesors();
    YCode   closeProcesors();
    YCode   createProcessors();
    YCode   initProcesors();
    YCode   connectProcessors();
    void    freeProcesors();

private:

    // ?
    std::list<YAbstractMedia*>          _data_processors_context;
    std::list<YAbstractCodec*>          _data_processors_codec;
    std::list<YAbstractFrameProcessor*> _data_processors_refi;

    // General
    volatile bool       _paused; 
    bool                _inited;

    int64_t             _options;

    int64_t             _source_video_stream_index;
    int64_t             _source_audio_stream_index;
    int64_t             _destination_video_stream_index;
    int64_t             _destination_audio_stream_index;

};
