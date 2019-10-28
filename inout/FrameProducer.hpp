#pragma once
#include "base/TemplateProcessor.hpp"
#include "base/Packet.hpp"
#include "base/Frame.hpp"

namespace fpp {

    class FrameProducer : public TemplateProcessor<Packet,Frame> {

    public:
        using FrameProducerList = std::list<FrameProducer*>;
    };

} // namespace fpp
