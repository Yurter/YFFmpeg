#pragma once
#include <base/Frame.hpp>
#include <stream/VideoParameters.hpp>

namespace ffmpeg { extern "C" {
    #include <libswscale/swscale.h>
} } // namespace ffmpeg

namespace fpp {

    using SharedSwsContext = std::shared_ptr<ffmpeg::SwsContext>;

    class RescalerContext : public Object {

    public:

        RescalerContext(IOParams parameters);
        virtual ~RescalerContext() override = default;

        Frame               scale(Frame source_frame);

        const IOParams      params;

    private:

        virtual void        init() override;
        Frame               createFrame() const;

    private:

        SharedSwsContext    _rescaler_context;

    };

} // namespace fpp
