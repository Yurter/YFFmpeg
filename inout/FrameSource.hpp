#pragma once
#include "base/TemplateProcessor.hpp"
#include "base/Frame.hpp"

namespace fpp {

    class FrameSource : public TemplateProcessor<Frame,Frame> {

    public:

        FrameSource();

    };

} // namespace fpp

