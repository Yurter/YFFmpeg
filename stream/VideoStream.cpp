//#include "VideoStream.hpp"
//#include "core/utils.hpp"

//namespace fpp {

//    VideoStream::VideoStream(VideoParameters* param) :
//        VideoStream(nullptr, param)
//    {
//        EMPTY_CONSTRUCTOR
//    }

//    VideoStream::VideoStream(AVStream* stream, VideoParameters* param) :
//        Stream(stream, param)
//    {
//        setName("VideoStream");
//    }

//    Code VideoStream::init() {
//        try_to(Stream::init());
//        auto video_parameers = dynamic_cast<VideoParameters*>(params);

//        return_if(not_inited_q(video_parameers->timeBase()),  Code::INVALID_INPUT);
//        return_if(not_inited_q(video_parameers->frameRate()), Code::INVALID_INPUT);

////        int64_t packet_duration = int64_t((1.0 / av_q2d(video_parameers->timeBase())) / av_q2d(video_parameers->frameRate()));
//        int64_t packet_duration = int64_t(1000.0 / av_q2d(video_parameers->frameRate())); //TODO

//        return_if(packet_duration < 1, Code::ERR);

//        _packet_dts_delta = packet_duration;
//        _packet_pts_delta = packet_duration;
//        _packet_duration = packet_duration;
//        setInited(true);
//        return Code::OK;
//    }

//    bool VideoStream::operator>(const Stream& other) const {
//        auto this_params = static_cast<VideoParameters*>(params);
//        auto other_params = static_cast<VideoParameters*>(other.params);
//        auto this_pixels = this_params->width() * this_params->height();
//        auto other_pixels = other_params->width() * other_params->height();
//        return this_pixels > other_pixels;
//    }

//} // namespace fpp
