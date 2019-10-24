#pragma once
#include "base/TemplateProcessor.hpp"
#include "base/Frame.hpp"

namespace fpp {

    class FrameSink : public TemplateProcessor<Frame,Frame> {

    public:

        FrameSink();

    };

} // namespace fpp
