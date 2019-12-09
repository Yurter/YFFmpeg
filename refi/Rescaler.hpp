#pragma once
#include "inout/FrameProcessor.hpp"
#include "stream/VideoStream.hpp"

namespace fpp {

    class Rescaler : public FrameProcessor {

    public:

        Rescaler(IOParams params);
        virtual ~Rescaler() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual Code        processInputData(Frame input_data) override;
        virtual bool        equalTo(const Processor * const other) const override;

        const IOParams      params;

    private:

        SwsContext*         _rescaler_context;

    };

} // namespace fpp
