#pragma once
#include <inout/PacketSink.hpp>

namespace fpp {

    class CustomPacketSink : public PacketSink {

    public:

        CustomPacketSink(const std::string& sink_name
                           , StreamVector streams
                           , const WriteFunction write_func
                           , const ProcessFunction process_func = [](Packet&) { return Code::OK; });
        virtual ~CustomPacketSink() override;

        virtual void        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

    private:

        virtual Code        processInputData(Packet input_data) override;
        virtual Code        writeOutputData(Packet output_data) override;

    private:

        const std::string       _sink_name;
        const WriteFunction     _write_func;
        const ProcessFunction   _process_func;

    };

    using CustomPacketSinkList = std::list<CustomPacketSink*>;

} // namespace fpp
