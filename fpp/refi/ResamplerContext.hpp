#pragma once
#include <fpp/base/Frame.hpp>
#include <fpp/stream/AudioParameters.hpp>

extern "C" {
    #include <libswresample/swresample.h>
}

namespace fpp {

    using SharedSwrContext = std::shared_ptr<SwrContext>;

    class ResamplerContext : public Object {

    public:

        ResamplerContext(IOParams parameters);
        virtual ~ResamplerContext() override = default;

        FrameList           resample(Frame source_frame);

        const IOParams      params;

    private:

        virtual void        init() override;
        Frame               createFrame() const;

    private:

        SharedSwrContext    _resampler_context;

    };

} // namespace fpp
