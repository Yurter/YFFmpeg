#pragma once

#include "context/YSource.hpp"
#include "context/YDestination.hpp"
#include "codec/YDecoder.hpp"
#include "codec/YEncoder.hpp"
#include "refi/YRescaler.hpp"
#include "refi/YResampler.hpp"
#include "refi/YVideoFilter.hpp"
#include "refi/YAudioFilter.hpp"
#include "YMap.hpp"

typedef std::list<YObject*>         ProcessorList;
typedef std::list<YSource*>         SourceList;
typedef std::list<YDestination*>    DestinationList;
typedef std::list<YContext*>        ContextList;
typedef std::list<YEncoder*>        EncoderList;
typedef std::list<YDecoder*>        DecoderList;
typedef std::list<YRefi*>           RefiList;

class YFFmpeg : public YThread
{

public:

    YFFmpeg(); //TODO hwaccel flag_enum|set_method
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

    // TODO

    YStream*            findBestInputStream(YMediaType media_type);
    StreamList          getOutputStreams(YMediaType media_type);

    YCode               connectIOStreams(YMediaType media_type);

    ContextList         contexts();
    SourceList          sources();
    DestinationList     destinations();
    DecoderList         decoders();
    EncoderList         encoders();
    RefiList            refis();

private:

    ProcessorList       _data_processors;

    std::list<std::list<YObject*>>      _processor_sequences; //TODO перенести внутрь YMap

    YMap*         _stream_map;

    // General
    volatile bool       _paused; 
    int64_t             _options;

};
