#pragma once
#include <fpp/base/Frame.hpp>
#include <fpp/stream/VideoParameters.hpp>

extern "C" {
    #include <libswscale/swscale.h>
}

namespace fpp {

    using SharedSwsContext = std::shared_ptr<SwsContext>;

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
