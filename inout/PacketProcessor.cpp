#include "PacketProcessor.hpp"

namespace fpp {

    PacketProcessor::PacketProcessor(ProcessorType type) :
        TemplateProcessor<Packet,Packet>(type)
    {
        setName("PacketProcessor");
    }

} // namespace fpp
