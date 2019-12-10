#pragma once
#include "FrameProcessor.hpp"

namespace fpp {

    class FrameSource : public FrameProcessor {

    public:

        FrameSource();

    };

    using FrameSourcePtr = std::unique_ptr<FrameSource>;

} // namespace fpp

