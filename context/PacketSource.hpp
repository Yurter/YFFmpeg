#pragma once
#include "../base/PacketProcessor.hpp"

namespace fpp {

    class PacketProducer : public TemplateProcessor<Packet,Packet> {

    public:

        PacketProducer();
        virtual ~PacketProducer() override;

    private:

        Code                readPacket();

    };

} // namespace fpp
