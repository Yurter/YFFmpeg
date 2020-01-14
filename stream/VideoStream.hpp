#pragma once
#include "base/Stream.hpp"
#include "VideoParameters.hpp"

namespace fpp {

    class VideoStream : public Stream { //TODO свернуть наследование ? да 14.01

    public:

        VideoStream(VideoParameters* param = new VideoParameters());
        VideoStream(AVStream* stream, VideoParameters* param);
        virtual ~VideoStream() override = default;

        virtual Code        init() override;

        bool                operator>(const Stream& other) const override; //TODO сравнивать параметры, а не потоки 14.01

    };

    //typedef std::list<VideoStream*> VideoStreamList;
    using VideoStreamList = std::list<VideoStream*>;

} // namespace fpp
