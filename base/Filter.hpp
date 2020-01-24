#pragma once
#include <inout/FrameProcessor.hpp>

namespace ffmpeg {
    class AVFilterGraph;
    class AVFilterContext;
    class AVFilterContext;
} // namespace ffmpeg

namespace fpp {

    class Filter : public FrameProcessor { //TODO определиться: фильтровать по выходным параметрам или входным, или настраивать? от этого зависит порядко на пайплайне

    public:

        Filter(IOParams params, const std::string& filters_descr);
        virtual ~Filter() override;

        virtual void        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual bool        equalTo(const SharedProcessor other) const override final;

        std::string         description() const;

        const IOParams      params;

    public:

        static const char   Separator = ',';
        static std::string  set_pts(float coef) { return "setpts=" + std::to_string(1.f / coef) + "*PTS"; }
        static std::string  keep_every_frame(int n) { return "select='not(mod(n," + std::to_string(n) + "))'"; }

    private:

        virtual Code        processInputData(Frame input_data) override;

    private:

        const std::string   _filters_descr;
        ffmpeg::AVFilterGraph*      _filter_graph;      //TODO убрать голый указатель 15.01
        ffmpeg::AVFilterContext*    _buffersrc_ctx;     //TODO убрать голый указатель 15.01
        ffmpeg::AVFilterContext*    _buffersink_ctx;    //TODO убрать голый указатель 15.01

    };

} // namespace fpp
