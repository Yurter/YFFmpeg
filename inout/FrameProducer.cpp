#include "FrameProducer.hpp"

namespace fpp {

    FrameProducer::FrameProducer() :
        TemplateProcessor<Packet,Frame>(ProcessorType::Process)
    {
        setName("FrameProducer");
    }

} // namespace fpp
