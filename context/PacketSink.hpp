#pragma once
#include "../base/PacketProcessor.hpp"

namespace fpp {

    class PacketSink : public TemplateProcessor<Packet,Packet> {

    public:

        PacketSink();
        virtual ~PacketSink() override;

//        virtual Code writeSourcePacket(Packet output_data) = 0;

    };

} // namespace fpp
