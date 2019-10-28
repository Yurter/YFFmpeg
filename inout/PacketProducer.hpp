#pragma once
#include "base/TemplateProcessor.hpp"
#include "base/Packet.hpp"
#include "base/Frame.hpp"

namespace fpp {

    class PacketProducer : public TemplateProcessor<Frame,Packet> {

    public:

        PacketProducer();

        using PacketProducerList = std::list<PacketProducer*>;

    };

} // namespace fpp
