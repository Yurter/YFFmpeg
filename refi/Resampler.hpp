#pragma once
#include "base/Refi.hpp"
#include "../control/AudioStream.hpp"

namespace fpp {

    class Resampler : public Refi {

    public:

        Resampler(StreamPair audio_streams);
        virtual ~Resampler() override;

        virtual Code       init() override;

    private:

        virtual Code        processInputData(Frame& input_data) override;
        bool                initOutputFrame(AVFrame** frame, int frame_size);
        bool                configChanged(AVFrame* in, AVFrame* out);

    protected:

        SwrContext*         _resampler_context;

    };

} // namespace fpp
