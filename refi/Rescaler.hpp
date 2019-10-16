#pragma once
#include "base/ffmpeg.hpp"
#include "base/Refi.hpp"
#include "../control/VideoStream.hpp"

namespace fpp {

    class Rescaler : public Refi {

    public:

        Rescaler(StreamPair audio_streams);
        virtual ~Rescaler() override;

        virtual Code        init() override;
        virtual Code        processInputData(Frame& input_data) override;

    private:

        SwsContext*         _rescaler_context;

    };

} // namespace fpp