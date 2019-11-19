#pragma once
#include "inout/FrameProcessor.hpp"

namespace fpp {

    class Filter : public FrameProcessor {

    public:

        Filter(const Parameters* input_data_params, std::string filters_descr);
        virtual ~Filter() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;

    private:

        virtual Code        processInputData(Frame input_data) override;

    private:

        const Parameters*   _input_data_params;
        std::string         _filters_descr;
        AVFilterGraph*      _filter_graph;
        AVFilterContext*    _buffersrc_ctx;
        AVFilterContext*    _buffersink_ctx;

    };

} // namespace fpp
