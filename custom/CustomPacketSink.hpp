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

        StreamVector        streams();

    private:

        virtual Code        processInputData(Packet input_data) override;
        virtual Code        writeOutputData(Packet output_data) override;

    private:

        uint64_t                        _context_uid;
        std::string                     _sink_name;
        StreamVector                    _streams;
        std::function<Code(Packet&)>    _write_func;
        std::function<Code(Packet&)>    _process_func;

    };

    using CustomPacketSinkList = std::list<CustomPacketSink*>;

} // namespace fpp
