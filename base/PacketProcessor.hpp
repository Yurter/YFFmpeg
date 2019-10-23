#pragma once
#include "TemplateProcessor.hpp"
#include "Frame.hpp"
#include "Packet.hpp"

namespace fpp {

    class PacketProcessor: public TemplateProcessor<Packet,Packet> {

    public:

        PacketProcessor();
        void setInputFunction(const std::function<void()> input_function);

    };

} // namespace fpp
