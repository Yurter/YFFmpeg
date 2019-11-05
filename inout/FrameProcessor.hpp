#pragma once
#include "base/TemplateProcessor.hpp"
#include "base/Frame.hpp"
#include "stream/VideoStream.hpp"
#include "stream/AudioStream.hpp"

namespace fpp {

    class FrameProcessor : public TemplateProcessor<Frame,Frame> {

    public:

        FrameProcessor();

    };

    using FrameProcessorList = std::list<FrameProcessor*>;

} // namespace fpp
