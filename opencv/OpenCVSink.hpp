#pragma once
#include "inout/FrameSink.hpp"

namespace fpp {

    class OpenCVSink : public FrameSink {

    public:

        OpenCVSink(const std::string mrl);
        virtual ~OpenCVSink() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

        OutputFormatContext& outputFormatContext();

    private:

        virtual Code        processInputData(Frame input_data) override;
        virtual Code        writeOutputData(Frame output_data) override;
        virtual Code        onStop() override;

    private:

        OutputFormatContext _output_format_context;


        // TemplateProcessor interface
    protected:
    };

    using MediaSinkList = std::list<MediaSink*>;

} // namespace fpp
