#pragma once
#include "inout/FrameSink.hpp"

namespace fpp {

    class CustomFrameSink : public FrameSink {

    public:

        CustomFrameSink(std::string sink_name
                           , StreamVector streams
                           , std::function<Code(Frame&)> write_func
                           , std::function<Code(Frame&)> process_func);
        virtual ~CustomFrameSink() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

        StreamVector        streams();

    private:

//        virtual Code        processInputData(Packet input_data) override;
//        virtual Code        writeOutputData(Packet output_data) override;

    private:

        std::string         _sink_name; //TODO перенести в Processor meta_data
        StreamVector        _streams;
        WriteFunction       _write_func;
        ProcessFunction     _process_func;

    };

//    using CustomPacketSinkList = std::list<CustomPacketSink*>;

} // namespace fpp
