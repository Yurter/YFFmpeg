#pragma once
#include "inout/FrameProcessor.hpp"

namespace fpp {

    class Filter : public FrameProcessor {

    public:

        Filter(IOParams params, std::string filters_descr);
        virtual ~Filter() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual bool        equalTo(const Processor * const other) const override final;

        std::string         description() const;

        const IOParams      params;

    private:

        virtual Code        processInputData(Frame input_data) override;

    private:

        std::string         _filters_descr;
        AVFilterGraph*      _filter_graph;
        AVFilterContext*    _buffersrc_ctx;
        AVFilterContext*    _buffersink_ctx;

    };

} // namespace fpp
