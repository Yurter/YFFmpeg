#pragma once
#include "inout/FrameSink.hpp"
#include "opencv2/opencv.hpp"

namespace fpp {

    class OpenCVSink : public FrameSink {

    public:

        OpenCVSink(const std::string mrl, VideoParameters* video_params);
        virtual ~OpenCVSink() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

    private:

        virtual Code        processInputData(Frame input_data) override;
        virtual Code        writeOutputData(Frame output_data) override;
        virtual Code        onStop() override;

        cv::Mat             frameToMat(Frame frame);

    private:

        std::string         _sink_name;
        VideoParameters*    _video_params;

    };

    using OpenCVSinkList = std::list<OpenCVSink*>;

} // namespace fpp
