#pragma once
#include "inout/PacketSink.hpp"
#include "format/OutputFormatContext.hpp"

namespace fpp {

    class MediaSink : public PacketSink {

    public:

        MediaSink(const std::string mrl, IOType preset = IOType::Auto);
        virtual ~MediaSink() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

        OutputFormatContext& outputFormatContext();

        void setRealTimeStamp(bool value) { _realtime_stamp = value; }

    private:

        virtual Code        processInputData(Packet input_data) override;
        virtual Code        writeOutputData(Packet output_data) override;
        virtual Code        onStop() override;

    private:

        OutputFormatContext _output_format_context;

        bool    _realtime_stamp = false;

    };

    using MediaSinkList = std::list<PacketSink*>;

} // namespace fpp
