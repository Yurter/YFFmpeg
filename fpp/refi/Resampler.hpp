#pragma once
#include <fpp/inout/FrameProcessor.hpp>

namespace ffmpeg { extern "C" {
    #include <libswresample/swresample.h>
} } // namespace ffmpeg

namespace fpp {

    using SharedSwrContext = std::shared_ptr<ffmpeg::SwrContext>;

    class Resampler : public FrameProcessor {

    public:

        Resampler(IOParams params);
        virtual ~Resampler() override = default;

        virtual void        init()  override;
        virtual Code        open()  override;
        virtual Code        close() override;

        const IOParams      params;

    private:

        virtual Code        processInputData(Frame input_data) override;
        bool                initOutputFrame(Frame& frame, int64_t frame_size);
        bool                configChanged(ffmpeg::AVFrame* in, ffmpeg::AVFrame* out); //TODO убрать постоянную проверку?

    protected:

        SharedSwrContext    _resampler_context;

    };

} // namespace fpp
