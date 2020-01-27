#pragma once
#include <fpp/base/TemplateProcessor.hpp>
#include <fpp/base/Packet.hpp>

namespace fpp {

    class PacketProcessor : public TemplateProcessor<Packet,Packet> { //TODO заменить на "using PacketProcessor = TemplateProcessor<Packet,Packet>;" ? (при отсутствии доп кода в релаизации класса)

    public:

        PacketProcessor();

    };

} // namespace fpp
