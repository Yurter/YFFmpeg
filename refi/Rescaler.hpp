#pragma once
#include "inout/FrameProcessor.hpp"
#include "stream/VideoStream.hpp"

namespace fpp {

    class Rescaler : public FrameProcessor {

    public:

        Rescaler(StreamPair video_streams);
        virtual ~Rescaler() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual Code        processInputData(Frame input_data) override;

    private:

        StreamPair          _io_streams;
        SwsContext*         _rescaler_context;

    };

} // namespace fpp
