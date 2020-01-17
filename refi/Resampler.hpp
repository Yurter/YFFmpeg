#pragma once
#include "inout/FrameProcessor.hpp"

namespace fpp {

    using SharedSwrContext = std::shared_ptr<SwrContext>;

    class Resampler : public FrameProcessor {

    public:

        Resampler(IOParams params);
        virtual ~Resampler() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;

        const IOParams      params;

    private:

        virtual Code        processInputData(Frame input_data) override;
        bool                initOutputFrame(Frame& frame, int64_t frame_size);
        bool                configChanged(AVFrame* in, AVFrame* out); //TODO убрать постоянную проверку?

    protected:

        SwrContext*         _resampler_context; //TODO убрать голый указатель 15.01

    };

} // namespace fpp
