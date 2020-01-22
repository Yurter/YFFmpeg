#pragma once
#include "inout/FrameProcessor.hpp"

extern "C" {
    #include <libswresample/swresample.h>
}

namespace fpp {

    using SharedSwrContext = std::shared_ptr<SwrContext>;

    class Resampler : public FrameProcessor {

    public:

        Resampler(IOParams params);
        virtual ~Resampler() override = default;

        virtual Code        init()  override;
        virtual Code        open()  override;
        virtual Code        close() override;

        const IOParams      params;

    private:

        virtual Code        processInputData(Frame input_data) override;
        bool                initOutputFrame(Frame& frame, int64_t frame_size);
        bool                configChanged(AVFrame* in, AVFrame* out); //TODO убрать постоянную проверку?

    protected:

        SharedSwrContext    _resampler_context;

    };

} // namespace fpp
