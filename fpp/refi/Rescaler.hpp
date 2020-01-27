#pragma once
#include <fpp/inout/FrameProcessor.hpp>

extern "C" {
    #include <libswscale/swscale.h>
}

namespace fpp {

    using SharedSwsContext = std::shared_ptr<SwsContext>;

    class Rescaler : public FrameProcessor { //TODO определиться: фильтровать по выходным параметрам или входным, или настраивать?

    public:

        Rescaler(IOParams params);
        virtual ~Rescaler() override;

        virtual void        init()  override;
        virtual Code        open()  override;
        virtual Code        close() override;
        virtual Code        processInputData(Frame input_data) override;
        virtual bool        equalTo(const SharedProcessor other) const override;

        const IOParams      params;

    private:

        SharedSwsContext    _rescaler_context;

    };

} // namespace fpp
