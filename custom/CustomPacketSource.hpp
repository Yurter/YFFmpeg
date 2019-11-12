#pragma once
#include "inout/PacketSource.hpp"

namespace fpp {

    class CustomPacketSource : public PacketSource {

    public:

        CustomPacketSource(std::string source_name
                           , StreamVector streams
                           , std::function<Packet()> read_func
                           , std::function<Code(Packet&)> process_func);
        virtual ~CustomPacketSource() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

        StreamVector        streams();

    private:

        virtual Code        readInputData(Packet& input_data) override;
        virtual Code        processInputData(Packet input_data) override;
        virtual Code        onStop() override;

        Code                sendEofPacket();

        std::string                     _source_name;
        StreamVector                    _streams;
        std::function<Packet()>         _read_func;
        std::function<Code(Packet&)>    _process_func;

    };

} // namespace fpp
