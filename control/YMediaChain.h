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

    bool    stop();                             ///<
    void    pause();                            ///<
    void    unpause();                          ///<

    void    setOptions(int64_t options);        ///<
    void    addElement(YObject* element);       ///<
    void    setRoute(streams_pair streams);     ///<

private:

    YCode   run() override;

    YCode   init();
    bool    inited() const;
    void    setInited(bool inited);
    bool    option(YOption option) const;

    YCode   initRefi();
    YCode   initCodec();
    YCode   openContext();
    YCode   closeContext();
    YCode   startProcesors();
    YCode   stopProcesors();
    YCode   determineSequences();
    YCode   checkProcessors();
    void    freeProcesors();

    bool    rescalerRequired(streams_pair streams);
    bool    resamplerRequired(streams_pair streams);

    bool contingencyVideoSourceRequired();
    bool contingencyAudioSourceRequired();


    void completeDestinationParametres();










private:

    // ?
    std::list<YAbstractMedia*>          _data_processors_context;
    std::list<YAbstractCodec*>          _data_processors_codec;
    std::list<YAbstractFrameProcessor*> _data_processors_refi;

    std::list<std::list<YObject*>>      _processor_sequences; //TODO зачем их хранить?

    YStreamMap*         _stream_map;

    // General
    volatile bool       _paused; 
    bool                _inited;

    int64_t             _options;

    int64_t             _source_video_stream_index;
    int64_t             _source_audio_stream_index;
    int64_t             _destination_video_stream_index;
    int64_t             _destination_audio_stream_index;

};
