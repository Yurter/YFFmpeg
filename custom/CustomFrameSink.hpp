#pragma once
#include "inout/FrameSink.hpp"

namespace fpp {

    class CustomFrameSink : public FrameSink {

    public:

        CustomFrameSink(std::string sink_name
                           , StreamVector streams
                           , std::function<Code(Packet&)> write_func
                           , std::function<Code(Packet&)> process_func);
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

        int64_t                         _context_uid; //TODO перенести в Processor
        std::string                     _sink_name;
        StreamVector                    _streams;
        std::function<Code(Packet&)>    _write_func;
        std::function<Code(Packet&)>    _process_func;

    };

//    using CustomPacketSinkList = std::list<CustomPacketSink*>;

} // namespace fpp
