#include "FrameProcessor.hpp"

namespace fpp {

    FrameProcessor::FrameProcessor() :
        TemplateProcessor<Frame,Frame>(ProcessorType::Process)
    {
        setName("FrameProcessor");
    }

} // namespace fpp
