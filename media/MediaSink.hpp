#pragma once
#include "inout/PacketSink.hpp"
#include "format/OutputFormatContext.hpp"
#include "core/time/Timer.hpp"

namespace fpp {

    class MediaSink : public PacketSink {

    public:

        MediaSink(const std::string mrl, IOType preset = IOType::Auto);
        virtual ~MediaSink() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;
        virtual bool        equalTo(const ProcessorPointer other) const override;

        const OutputFormatContext* outputFormatContext() const;

        void                setFlushTimeout(int64_t msec);

    private:

        virtual Code        processInputData(Packet input_data) override;
        virtual Code        writeOutputData(Packet output_data) override;
        virtual Code        onStop() override;

    private:

        OutputFormatContext _output_format_context;

        Timer               _flush_timer;
        int64_t             _flush_timeout_ms;

    };

} // namespace fpp
