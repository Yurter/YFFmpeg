#pragma once
#include "opencv/OpenCVSink.hpp"
#include "custom/CustomPacketSource.hpp"
#include "custom/CustomPacketSink.hpp"
#include "custom/CustomFrameSource.hpp"
#include "custom/CustomFrameSink.hpp"
#include "media/MediaSource.hpp"
#include "media/MediaSink.hpp"
#include "codec/Decoder.hpp"
#include "codec/Encoder.hpp"
#include "refi/Rescaler.hpp"
#include "refi/Resampler.hpp"
#include "refi/VideoFilter.hpp"
#include "refi/AudioFilter.hpp"
#include "base/Route.hpp"

namespace fpp {

    using MetaStream = std::pair<uint64_t,uint64_t>;
    using MetaRoute = std::pair<MetaStream,MetaStream>;
    using MetaMap = std::list<MetaRoute>;

    using ProcessorSequence = std::list<Processor*>;

    class Pipeline : public Thread {

    public:

        Pipeline(); //TODO hwaccel flag_enum|set_method
        virtual ~Pipeline() override;

        bool                stopPr();                             ///< Функция прерывает работу класса.

        Code                addElement(FrameSource*     frame_source);
        Code                addElement(FrameSink*       frame_sink);
        Code                addElement(PacketSource*    packet_source);
        Code                addElement(PacketSink*      packet_sink);

        void                remElement(Processor* processor);

        void                setOptions(int64_t options);        ///< Функция устанавливает переданные ей опции Option.
        void                dump() const;                       ///< TODO description

    private:

        virtual Code        init()      override;
        virtual Code        run()       override;
        virtual Code        onStart()   override;
        virtual Code        onStop()    override;

        bool                option(Option option) const;

        Code                checkFormatContexts();

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
        void                freeProcesors();

        Code                createSequence(Route& route);

        Code                simplifyRoutes();

        Stream*             findStream(int64_t uid);
        Code                determineSequence(Object* output_processor);

        virtual std::string toString() const override;

        void pushproc(Processor* processor);
        void remproc(Processor* processor);

        // TODO

        Stream*             findBestInputStream(MediaType media_type);
        StreamList          getOutputStreams(MediaType media_type);

        Route               findRoute(Processor* processor);

        MediaSourceList     mediaSources()  const;
        MediaSinkList       mediaSinks()    const;
        OpenCVSinkList      openCVSinks()   const;
        DecoderList         decoders()      const;
        EncoderList         encoders()      const;

    private:

        ProcessorList       _processors;

        std::mutex          _processor_mutex;


        MetaMap             _metamap;

        RouteList           _route_list;

//        int64_t             _options;
    };

} // namespace fpp
