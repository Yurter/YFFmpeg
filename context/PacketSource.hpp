#pragma once
#include "../base/PacketProcessor.hpp"

namespace fpp {

    class PacketProducer : public TemplateProcessor<Packet,Packet> {

    public:

        PacketProducer();
        virtual ~PacketProducer() override;

        Code                readSourcePacket();

    };

} // namespace fpp
