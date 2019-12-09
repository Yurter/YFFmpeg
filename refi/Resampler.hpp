#pragma once
#include "inout/FrameProcessor.hpp"
#include "stream/AudioStream.hpp"

namespace fpp {

    class Resampler : public FrameProcessor {

    public:

        Resampler(IOParams params);
        virtual ~Resampler() override;

        virtual Code       init() override;

        const IOParams      params;

    private:

        virtual Code        processInputData(Frame input_data) override;
        bool                initOutputFrame(AVFrame** frame, int frame_size);
        bool                configChanged(AVFrame* in, AVFrame* out);

    protected:

        SwrContext*         _resampler_context;

    };

} // namespace fpp
