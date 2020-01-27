#pragma once
#include <fpp/base/TemplateProcessor.hpp>
#include <fpp/base/Packet.hpp>
#include <fpp/base/Frame.hpp>

namespace fpp {

    class FrameProducer : public TemplateProcessor<Packet,Frame> {

    public:

        FrameProducer();

    };

} // namespace fpp
