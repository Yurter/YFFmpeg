#include "OpenCVSink.hpp"

namespace fpp {

    OpenCVSink::OpenCVSink(const std::string mrl, VideoStream* video_stream) :
        _sink_name(mrl)
      , _video_stream(video_stream)
      , _video_params(static_cast<VideoParameters*>(video_stream->parameters))
    {
        setName("OpenCVSink");
        _video_params->setPixelFormat(AV_PIX_FMT_BGR24);
        _video_stream->setContext(this);
    }

    OpenCVSink::~OpenCVSink() {
        try_throw(close());
    }

    Code OpenCVSink::init() {
//        cv::namedWindow(_sink_name, cv::WINDOW_AUTOSIZE);
//        cv::moveWindow(_sink_name, 0, 0);
        setInited(true);
        return Code::OK;
    }

    Code OpenCVSink::open() {
        return Code::OK;
    }

    Code OpenCVSink::close() {
        cv::destroyWindow(_sink_name);
        return Code::OK;
    }

    std::string OpenCVSink::toString() const {
        return "OpenCVSink TODO";
    }

    VideoStream* OpenCVSink::videoStream() {
        return _video_stream;
    }

    Code OpenCVSink::processInputData(Frame input_data) {
        try_to(storeOutputData(input_data));
        return Code::OK;
    }

    Code OpenCVSink::writeOutputData(Frame output_data) {
        static int counter = 0;
//        cv::imwrite("debug_images/" + _sink_name + std::to_string(counter++)  + ".png", frameToMat(output_data));
//        cv::imshow("_sink_name", frameToMat(output_data).clone());
//        utils::sleep_for(45);
//        cv::waitKey(5);
        return Code::OK;
    }

    Code OpenCVSink::onStop() {
        try_to(close());
        return Code::OK;
    }

    cv::Mat OpenCVSink::frameToMat(Frame frame) {
        return cv::Mat(
            int(_video_params->height())
            , int(_video_params->width())
            , CV_8UC3
            , frame.raw()->data[0]
            , size_t(frame.raw()->linesize[0])
        );
    }

} // namespace fpp
