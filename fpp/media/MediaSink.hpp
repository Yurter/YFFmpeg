#pragma once
#include <fpp/context/OutputFormatContext.hpp>
#include <fpp/inout/PacketSink.hpp>
#include <fpp/core/time/Timer.hpp>

namespace fpp {

    class MediaSink : public PacketSink {

    public:

        MediaSink(const std::string& mrl, Preset preset = Preset::Auto);
        virtual ~MediaSink() override;

        virtual void        init()  override;
        virtual Code        open()  override;
        virtual Code        close() override;
        virtual std::string toString() const override;
        virtual bool        equalTo(const SharedProcessor other) const override;

        const OutputFormatContext* outputFormatContext() const;

    private:

        virtual Code        processInputData(Packet input_data) override;
        virtual Code        writeOutputData(Packet output_data) override;

    private:

        OutputFormatContext _output_format_context;
        Timer               _flush_timer;

    };

} // namespace fpp
