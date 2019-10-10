#pragma once
#include "base/ffmpeg.hpp"
#include "base/Refi.hpp"
#include "../control/VideoStream.hpp"

namespace fpp {

    class Rescaler : public Refi {

    public:

        Rescaler(StreamPair audio_streams);
        virtual ~Rescaler() override;

        Code                init() override;
        Code                processInputData(Frame& input_data) override;

    private:

        //FFmpeg
        //

    };

} // namespace fpp
