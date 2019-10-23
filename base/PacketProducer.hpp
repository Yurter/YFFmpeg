#pragma once
#include "TemplateProcessor.hpp"
#include "Frame.hpp"
#include "Packet.hpp"

namespace fpp {

    class PacketProducer : public TemplateProcessor<Frame,Packet> {

    public:

        PacketProducer();

    };

} // namespace fpp
