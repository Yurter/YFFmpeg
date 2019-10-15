#pragma once
#include "Refi.hpp"
#include "Frame.hpp"

namespace fpp {

    class Filter : public Refi {

    public:

        Filter(StreamPair io_streams, std::string filters_descr);
        virtual ~Filter() override;

        virtual Code init() override;

    private:

        virtual Code processInputData(Frame& input_data) override;

    private:

        std::string         _filters_descr;
        AVFilterGraph*      _filter_graph;
        AVFilterContext*    _buffersrc_ctx;
        AVFilterContext*    _buffersink_ctx;

    };

} // namespace fpp
