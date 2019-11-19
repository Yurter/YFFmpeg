#include "FrameSource.hpp"

namespace fpp {

    FrameSource::FrameSource() :
        TemplateProcessor<Frame,Frame>(ProcessorType::Input)
    {
        setName("FrameSource");
    }

} // namespace fpp
