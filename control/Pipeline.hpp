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

    class Pipeline;
    using PipelinePointer = std::shared_ptr<Pipeline>;

    enum class SourceType {
        Mandatory,
        Backup,
    };

    class Pipeline : public Thread {

    public:

        Pipeline();
        virtual ~Pipeline() override;

        Code                addElement(ProcessorPointer processor, SourceType priority = SourceType::Mandatory); //TODO refactoring priority 13.01
        void                remElement(ProcessorPointer processor);
        void                remElement(const int64_t uid);

        void                dump() const;                       ///< TODO description

        Code                simplifyRoutes();

    private:

        virtual Code        init()      override;
        virtual Code        run()       override;
        virtual Code        onStart()   override;
        virtual Code        onStop()    override;

        Code                checkFormatContexts();

        Code                joinProcesors();

        Code                createSequence(Route& route);

        ProcessorPointer    findProcessorByUid(const int64_t uid);

        Stream*             findStream(int64_t uid);
        Code                determineSequence(const ProcessorPointer output_processor);

        virtual std::string toString() const override;

        Stream*             findBestInputStream(MediaType media_type);
        StreamList          getOutputStreams(MediaType media_type);

        Route*              findRoute(const int64_t uid);

    private:

        ProcessorList       _data_sources;
        ProcessorList       _data_sinks;
        ProcessorList       _data_backup_sources;

        RouteVector         _route_list;

    };

} // namespace fpp
