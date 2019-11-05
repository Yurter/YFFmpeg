#include "OpenCVSink.hpp"

namespace fpp {

    OpenCVSink::OpenCVSink(const std::string mrl, VideoParameters* video_params) :
        _sink_name(mrl)
      , _video_params(video_params)
    {
        setName("OpenCVSink");
    }

    OpenCVSink::~OpenCVSink() {
        try_throw(close());
    }

    Code OpenCVSink::init() {
        cv::namedWindow(_sink_name, cv::WINDOW_AUTOSIZE);
        cv::moveWindow(_sink_name, 0, 0);
        setInited(true);
        return Code::OK;
    }

    Code OpenCVSink::open() {
        return Code::OK;
    }

    Code OpenCVSink::close() {
        cv::destroyWindow(name());
        return Code::OK;
    }

    std::string OpenCVSink::toString() const {
        return "OpenCVSink TODO";
    }

    Code OpenCVSink::processInputData(Frame input_data) {
        try_to(storeOutputData(input_data));
        return Code::OK;
    }

    Code OpenCVSink::writeOutputData(Frame output_data) {
        cv::imshow(_sink_name, frameToMat(output_data));
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
