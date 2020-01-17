#pragma once
#include "inout/PacketSink.hpp"
#include "context/OutputFormatContext.hpp"
#include "core/time/Timer.hpp"

namespace fpp {

    class MediaSink : public PacketSink {

    public:

        MediaSink(const std::string& mrl, IOPreset preset = IOPreset::Auto);
        virtual ~MediaSink() override;

        virtual Code        init()  override;
        virtual Code        open()  override;
        virtual Code        close() override;
        virtual std::string toString() const override;
        virtual bool        equalTo(const SharedProcessor other) const override;

        const OutputFormatContext* outputFormatContext() const;

    private:

        virtual Code        processInputData(Packet input_data) override;
        virtual Code        writeOutputData(Packet output_data) override;
        virtual Code        onStop() override;

    private:

        OutputFormatContext _output_format_context;
        Timer               _flush_timer;

    };

} // namespace fpp
