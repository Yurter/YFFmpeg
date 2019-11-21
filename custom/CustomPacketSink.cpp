#include "CustomPacketSink.hpp"

namespace fpp {

    CustomPacketSink::CustomPacketSink(std::string sink_name
                                       , StreamVector streams
                                       , std::function<Code(Packet&)> write_func
                                       , std::function<Code(Packet&)> process_func) :
        _context_uid(utils::gen_context_uid())
      , _sink_name(sink_name)
      , _streams(streams)
      , _write_func(write_func)
      , _process_func(process_func)
    {
        setName("CustomPacketSink");
    }

    CustomPacketSink::~CustomPacketSink() {
        //
    }

    Code CustomPacketSink::init() {
        for (auto&& stream : _streams) {
//            auto avstream = avformat_new_stream(nullptr, stream->parameters->codec());
//            return_if(not_inited_ptr(avstream), Code::ERR);
//            stream->setRaw(avstream);

            stream->setUid(utils::gen_stream_uid(_context_uid, stream->parameters->streamIndex()));
            stream->setContext(this);
        }
        setInited(true);
        return Code::OK;
    }

    Code CustomPacketSink::open() {
        setOpened(true);
        return Code::OK;
    }

    Code CustomPacketSink::close() {
        setOpened(false);
        return Code::OK;
    }

    std::string CustomPacketSink::toString() const {
        return "TODO";
    }

    StreamVector CustomPacketSink::streams() {
        return _streams;
    }

    Code CustomPacketSink::processInputData(Packet input_data) {
        Code ret = _process_func(input_data);
        return_if(utils::exit_code(ret), ret);
        if (ret == Code::OK) {
            try_to(storeOutputData(input_data));
        }
        return ret;
    }

    Code CustomPacketSink::writeOutputData(Packet output_data) {
        try_to(_write_func(output_data));
        return Code::OK;
    }

} // namespace fpp
