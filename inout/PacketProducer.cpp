#include "PacketProducer.hpp"

namespace fpp {

    PacketProducer::PacketProducer() :
        TemplateProcessor<Frame,Packet>(ProcessorType::Process)
    {
        setName("PacketProducer");
    }

} // namespace fpp
