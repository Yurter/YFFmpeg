#pragma once
#include "opencv/OpenCVSink.hpp"
#include "custom/CustomPacketSource.hpp"
#include "media/MediaSource.hpp"
#include "media/MediaSink.hpp"
#include "codec/Decoder.hpp"
#include "codec/Encoder.hpp"
#include "refi/Rescaler.hpp"
#include "refi/Resampler.hpp"
#include "refi/VideoFilter.hpp"
#include "refi/AudioFilter.hpp"
#include "YMap.hpp"
#include "base/Route.hpp"

namespace fpp {

    using ProcessorList = std::list<Object*>;

    using MetaStream = std::pair<uint64_t,uint64_t>;
    using MetaRoute = std::pair<MetaStream,MetaStream>;
    using MetaMap = std::list<MetaRoute>;

//    using ProcessorSequence = std::list<Object*>;
    using ProcessorSequence = std::list<Processor*>;

    class Pipeline : public Thread {

    public:

        Pipeline(); //TODO hwaccel flag_enum|set_method
        virtual ~Pipeline() override;

        bool                stop();                             ///< Функция прерывает работу класса.
        void                pause();                            ///< Функция приостанавливает работу класса.
        void                unpause();                          ///< Функция возобновляет работу класса.

        void                setOptions(int64_t options);        ///< Функция устанавливает переданные ей опции Option.
        void                addElement(Object* element);        ///< Функция добавляет процессор медиа-данных в кучу.
        void                remElement(Object* element);     ///< Функция ...

        // До старта потоков нет возможности указать потоки явно
    //    void                setRoute(Stream* input_stream, Stream* output_stream);            ///< Функция устанавливает соответствие между входным и выходным потоками.
        void                setRoute(MediaSource* input_context, int64_t input_stream_index
                                     , MediaSink* output_context, int64_t output_stream_index);

        void                dump() const;                       ///< TODO description

    private:

        virtual Code        init()  override;
        virtual Code        run()   override;

        bool                option(Option option) const;

        Code                checkFormatContexts();

//        Code                initProcessors();

        Code                initMap();
        Code                initRefi();
        Code                initCodec();
        Code                initMedia();


        Code                openCodec();
        Code                openMediaSources();
        Code                openMediaSinks();

        Code                closeMediaSources();
        Code                closeMediaSinks();
        Code                startProcesors();
        Code                stopProcesors();
        Code                joinProcesors();
        Code                determineSequences();
        void                freeProcesors();

        Code                createSequence(Route route);

        Stream*             findStream(int64_t uid);
//        Code                determineSequence(MediaSink* media_sink); //TODO

        virtual std::string toString() const override;

        // TODO

        Stream*             findBestInputStream(MediaType media_type);
        StreamList          getOutputStreams(MediaType media_type);

        Code                connectIOStreams(MediaType media_type);

        MediaSourceList     mediaSources()  const;
        MediaSinkList       mediaSinks()    const;
        OpenCVSinkList      openCVSinks()   const;
        DecoderList         decoders()      const;
        EncoderList         encoders()      const;
//        FrameProcessor      refis()         const;

    private:

        ProcessorList       _processors;

                                         // DataFlow? Pipe? Pipeline?
        std::list<ProcessorSequence>      _processor_sequences; //TODO перенести внутрь YMap ?

        YMap*               _map;

        MetaMap             _metamap;

        RouteList           _route_list;

        // General
        volatile bool       _paused;
        int64_t             _options;

    };

} // namespace fpp
