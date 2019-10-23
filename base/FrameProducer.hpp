#pragma once
#include "TemplateProcessor.hpp"
#include "Packet.hpp"
#include "Frame.hpp"

namespace fpp {

    class FrameProducer : public TemplateProcessor<Packet,Frame> {

    public:

        FrameProducer();

    };

} // namespace fpp
