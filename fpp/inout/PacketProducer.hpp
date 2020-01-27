#pragma once
#include <fpp/base/TemplateProcessor.hpp>
#include <fpp/base/Packet.hpp>
#include <fpp/base/Frame.hpp>

namespace fpp {

    class PacketProducer : public TemplateProcessor<Frame,Packet> {

    public:

        PacketProducer();

    };

} // namespace fpp
