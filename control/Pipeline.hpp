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
                                                                                                                    // перенести свойство в процессор
        void                remElement(ProcessorPointer processor);
        void                remElement(int64_t uid);

        void                dump() const;

    private:

        virtual Code        init()      override;
        virtual Code        run()       override;
        virtual Code        onStart()   override;
        virtual Code        onStop()    override;

        Code                createSequence(Route& route);
        Code                determineSequence(const ProcessorPointer output_processor);


        ProcessorPointer    findProcessor(int64_t uid);
        StreamPointer       findStream(StreamId_t uid);
        StreamPointer       findBestInputStream(MediaType type);


        virtual std::string toString() const override;


        Route*              findRoute(const int64_t uid);

        Code                simplifyRoutes(); //TODO доаработать 15.01

    private:

        ProcessorList       _data_sources;
        ProcessorList       _data_sinks;
        ProcessorList       _data_backup_sources; //TODO убрать и объединить с обычными сорсами ? 14.01
        RouteVector         _route_list;

    };

} // namespace fpp
