#pragma once
#include <fpp/base/TemplateProcessor.hpp>
#include <fpp/base/Frame.hpp>

namespace fpp {

    class FrameProcessor : public TemplateProcessor<Frame,Frame> {

    public:

        FrameProcessor();

    };

} // namespace fpp
