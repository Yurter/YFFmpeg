#pragma once
#include "inout/FrameProcessor.hpp"
#include "stream/AudioStream.hpp"

namespace fpp {

    class Resampler : public FrameProcessor {

    public:

        Resampler(StreamPair audio_streams);
        virtual ~Resampler() override;

        virtual Code       init() override;

    private:

        virtual Code        processInputData(Frame input_data) override;
        bool                initOutputFrame(AVFrame** frame, int frame_size);
        bool                configChanged(AVFrame* in, AVFrame* out);

    protected:

        StreamPair          _io_streams;
        SwrContext*         _resampler_context;

    };

} // namespace fpp
