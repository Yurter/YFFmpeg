#pragma once
#include <base/Frame.hpp>
#include <stream/AudioParameters.hpp>

namespace ffmpeg { extern "C" {
    #include <libswresample/swresample.h>
} } // namespace ffmpeg

namespace fpp {

    using SharedSwrContext = std::shared_ptr<ffmpeg::SwrContext>;

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
