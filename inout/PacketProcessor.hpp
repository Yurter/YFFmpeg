#pragma once
#include "base/TemplateProcessor.hpp"
#include "base/Packet.hpp"

namespace fpp {

    class PacketProcessor : public TemplateProcessor<Packet,Packet> { //TODO заменить на "using PacketProcessor = TemplateProcessor<Packet,Packet>;" ?

    public:

        PacketProcessor();

    };

} // namespace fpp
