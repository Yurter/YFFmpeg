#pragma once
#include "base/Refi.hpp"
#include "../control/AudioStream.hpp"

namespace fpp {

    class YResampler : public Refi {

    public:

        YResampler(StreamPair audio_streams);
        virtual ~YResampler() override;

        virtual Code       init() override;

    private:

        virtual Code        processInputData(Frame& input_data) override;
        bool                initOutputFrame(AVFrame** frame, int frame_size);
        bool                configChanged(AVFrame* in, AVFrame* out);

    protected:

        // FFmpeg
        SwrContext*         _resampler_context;

    };

} // namespace fpp
