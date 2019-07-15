#pragma once

#include "context/YSource.h"
#include "context/YDestination.h"
#include "codec/YDecoder.h"
#include "codec/YEncoder.h"
#include "refi/YRescaler.h"
#include "refi/YResampler.h"
#include "refi/YVideoFilter.h"
#include "refi/YAudioFilter.h"
#include "YMap.h"

typedef std::list<YThread*> ProcessorList;

class YFFmpeg : public YThread
{

public:

    YFFmpeg(); //TODO hwaccel flag_enum|bool_arg|set_method
    ~YFFmpeg() override;

    bool                stop();                             ///< Функция завершает работу класса.
    void                pause();                            ///< Функция приостанавливает работу класса.
    void                unpause();                          ///< Функция возобновляет работу класса.

    void                setOptions(int64_t options);        ///< Функция устанавливает переданные ей опции.
    void                addElement(YObject* element);       ///< Функция добавляет процессор меди-данных в кучу.
    void                setRoute(Route route);              ///< Функция устанавливает соответствие между входным и выходным потоками.
    void                setRoute(stream_context input_stream_context, stream_context output_stream_context);
    void                setRoute(YContext* input_context, int64_t input_stream_index
                                 , YContext* output_context, int64_t output_stream_index);

    void                dump() const;                       ///<

private:

    YCode               init()  override;
    YCode               run()   override;

    bool                option(YOption option) const;

    YCode               initMap();
    YCode               initRefi();
    YCode               initCodec();
    YCode               initContext();
    YCode               openContext();
    YCode               closeContext();
    YCode               startProcesors();
    YCode               stopProcesors();
    YCode               joinProcesors();
    YCode               determineSequences();
    void                freeProcesors();

    std::string         toString() const override;

    bool                rescalerRequired(streams_pair streams);
    bool                resamplerRequired(streams_pair streams);

    bool                contingencyVideoSourceRequired();
    bool                contingencyAudioSourceRequired();

    void                completeDestinationParametres();

private:

    ProcessorList       _data_processors;

    // ? //TODO объянить?
    std::list<YContext*>          _data_processors_context;
//    std::list<YAbstractCodec*>          _data_processors_codec;
    std::list<YDecoder*>          _data_processors_decoder;
    std::list<YEncoder*>          _data_processors_encoder;
    std::list<YAbstractRefi*>           _data_processors_refi;

    std::list<std::list<YObject*>>      _processor_sequences; //TODO зачем их хранить?

    YMap*         _stream_map;

    // General
    volatile bool       _paused; 
    int64_t             _options;

};
