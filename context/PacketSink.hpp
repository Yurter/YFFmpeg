#pragma once
#include "../base/PacketProcessor.hpp"

namespace fpp {

    class PacketSink : public TemplateProcessor<Packet,Packet> {

    public:

        PacketSink();
        virtual ~PacketSink() override;

//        Code                writePacket();

    };

} // namespace fpp
