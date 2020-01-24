#pragma once
#include <inout/FrameProcessor.hpp>

namespace fpp {

    class FrameSink : public FrameProcessor {

    public:

        FrameSink();
        virtual ~FrameSink() override;

    private:

        Code                writeFrame();

    };

} // namespace fpp
