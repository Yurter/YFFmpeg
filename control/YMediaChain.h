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

    bool    stop();
    void    pause();
    void    unpause();

    void    setOptions(int64_t options);
    void    addElement(YObject* element);
    void    addRoute(streams_pair streams);

private:

    YCode   init();

    YCode   run() override;

    bool    rescalerRequired(streams_pair streams);
    bool    resamplerRequired(streams_pair streams);

    bool contingencyVideoSourceRequired();
    bool contingencyAudioSourceRequired();

    bool    option(YOption option);
    void parseOptions();

    void completeDestinationParametres();


    bool    inited() const;
    void    setInited(bool inited);



    YCode   initRefi();
    YCode   initCodec();

    YCode   openContext();
    YCode   closeContext();

    YCode   connectProcessors();

    YCode   startProcesors();
    YCode   stopProcesors();

    YCode   defaultRelation(std::list<streams_pair>* relation_list);
    YCode   determineSequences();

    YCode   ckeckProcessors();
    void    freeProcesors();




private:

    // ?
    std::list<YAbstractMedia*>          _data_processors_context;
    std::list<YAbstractCodec*>          _data_processors_codec;
    std::list<YAbstractFrameProcessor*> _data_processors_refi;

    std::list<std::list<YObject*>>      _processor_sequences;

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
