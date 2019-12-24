#pragma once
#include "inout/PacketSink.hpp"

namespace fpp {

    class CustomPacketSink : public PacketSink {

    public:

        CustomPacketSink(std::string sink_name
                           , StreamVector streams
                           , std::function<Code(Packet&)> write_func
                           , std::function<Code(Packet&)> process_func);
        virtual ~CustomPacketSink() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

    private:

        virtual Code        processInputData(Packet input_data) override;
        virtual Code        writeOutputData(Packet output_data) override;
        virtual Code        onStop() override;

    private:

        std::string                     _sink_name;
        std::function<Code(Packet&)>    _write_func;
        std::function<Code(Packet&)>    _process_func;

    };

    using CustomPacketSinkList = std::list<CustomPacketSink*>;

} // namespace fpp
