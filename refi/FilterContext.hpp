#pragma once
#include <base/Frame.hpp>
#include <stream/VideoParameters.hpp>

namespace ffmpeg {
    class AVFilterGraph;
    class AVFilterContext;
    class AVFilterContext;
} // namespace ffmpeg

namespace fpp {

    class FilterContext : public Object {

    public:

        FilterContext(SharedParameters parameters, const std::string& filters_descr);
        virtual ~FilterContext() override = default;

        FrameList           filter(Frame frame);
        std::string         description() const;

        const SharedParameters params;

    public:

        static const char   Separator = ',';
        static std::string  set_pts(float coef) { return "setpts=" + std::to_string(1.f / coef) + "*PTS"; }
        static std::string  keep_every_frame(int n) { return "select='not(mod(n," + std::to_string(n) + "))'"; }

    private:

        virtual void        init() override;

    private:

        const std::string   _filters_descr;
        ffmpeg::AVFilterGraph*      _filter_graph;      //TODO убрать голый указатель 15.01
        ffmpeg::AVFilterContext*    _buffersrc_ctx;     //TODO убрать голый указатель 15.01
        ffmpeg::AVFilterContext*    _buffersink_ctx;    //TODO убрать голый указатель 15.01

    };

} // namespace fpp
