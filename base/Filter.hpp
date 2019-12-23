#pragma once
#include "inout/FrameProcessor.hpp"

namespace fpp {

    class Filter : public FrameProcessor { //TODO определиться: фильтровать по выходным параметрам или входным, или настраивать?

    public:

        Filter(IOParams params, std::string filters_descr);
        virtual ~Filter() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual bool        equalTo(const ProcessorPointer other) const override final;

        std::string         description() const;

        const IOParams      params;

    private:

        virtual Code        processInputData(Frame input_data) override;

    private:
        int in_coint = 0;
        int out_coint = 0;

        std::string         _filters_descr;
        AVFilterGraph*      _filter_graph;
        AVFilterContext*    _buffersrc_ctx;
        AVFilterContext*    _buffersink_ctx;

    };

} // namespace fpp
