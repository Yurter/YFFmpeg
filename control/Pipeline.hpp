#pragma once
#include "context/Source.hpp"
#include "context/Sink.hpp"
#include "codec/Decoder.hpp"
#include "codec/Encoder.hpp"
#include "refi/Rescaler.hpp"
#include "refi/Resampler.hpp"
#include "refi/VideoFilter.hpp"
#include "refi/AudioFilter.hpp"
#include "YMap.hpp"

namespace fpp {

    using ProcessorList = std::list<Object*>;

    using MetaStream = std::pair<uint64_t,uint64_t>;
    using MetaRoute = std::pair<MetaStream,MetaStream>;
    using MetaMap = std::list<MetaRoute>;

    using ProcessorSequence = std::list<Object*>;

    class Pipeline : public Thread {

    public:

        Pipeline(); //TODO hwaccel flag_enum|set_method
        ~Pipeline() override;

        bool                stop();                             ///< Функция прерывает работу класса.
        void                pause();                            ///< Функция приостанавливает работу класса.
        void                unpause();                          ///< Функция возобновляет работу класса.

        void                setOptions(int64_t options);        ///< Функция устанавливает переданные ей опции Option.
        void                addElement(Object* element);       ///< Функция добавляет процессор медиа-данных в кучу.

        // До старта потоков нет возможности указать потоки явно
    //    void                setRoute(Stream* input_stream, Stream* output_stream);            ///< Функция устанавливает соответствие между входным и выходным потоками.
        void                setRoute(Context* input_context, int64_t input_stream_index
                                     , Context* output_context, int64_t output_stream_index);

        void                dump() const;                       ///< TODO description

    private:

        virtual Code        init()  override;
        virtual Code        run()   override;

        bool                option(Option option) const;

        Code                checkIOContexts();

        Code                initMap();
        Code                initRefi();
        Code                initCodec();
        Code                initContext();

//        Code                openContext();
        Code                openSources();
        Code                openSinks();

        Code                closeContexts();
        Code                startProcesors();
        Code                stopProcesors();
        Code                joinProcesors();
        Code                determineSequences();
        void                freeProcesors();

        virtual std::string toString() const override;

        // TODO

        Stream*             findBestInputStream(MediaType media_type);
        StreamList          getOutputStreams(MediaType media_type);

        Code                connectIOStreams(MediaType media_type);

        ContextList         contexts()      const;
        SourceList          sources()       const;
        SinkList            sinks()         const;
        DecoderList         decoders()      const;
        EncoderList         encoders()      const;
        RefiList            refis()         const;

    private:

        ProcessorList       _data_processors;

                                         // DataFlow? Pipe? Pipeline?
        std::list<std::list<Object*>>      _processor_sequences; //TODO перенести внутрь YMap ?

        YMap*               _map;

        MetaMap             _metamap;

        // General
        volatile bool       _paused;
        int64_t             _options;

    };

} // namespace fpp
