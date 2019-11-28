#pragma once
#include "opencv2/opencv.hpp"
#include "inout/FrameSink.hpp"

namespace fpp {

    class OpenCVSink : public FrameSink { //TODO удалить класс, есть CustomFrameSink, созранить пресет потока

    public:

        OpenCVSink(const std::string mrl, VideoStream* video_stream);
        virtual ~OpenCVSink() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

        VideoStream*        videoStream();

    private:

        virtual Code        processInputData(Frame input_data) override;
        virtual Code        writeOutputData(Frame output_data) override;
        virtual Code        onStop() override;

        cv::Mat             frameToMat(Frame frame);

    private:

        std::string         _sink_name;
        VideoStream*        _video_stream;
        VideoParameters*    _video_params;

    };

    using OpenCVSinkList = std::list<OpenCVSink*>;

} // namespace fpp
