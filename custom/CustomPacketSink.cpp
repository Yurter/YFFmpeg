#include "CustomPacketSink.hpp"

namespace fpp {

    CustomPacketSink::CustomPacketSink(std::string sink_name
                                       , StreamVector streams
                                       , WriteFunction write_func
                                       , ProcessFunction process_func)
        : _sink_name(sink_name)
        , _write_func(write_func)
        , _process_func(process_func) {
        setName("CustPktSink");
        setStreams(streams);
    }

    CustomPacketSink::~CustomPacketSink() {
        try_throw(close());
    }

    Code CustomPacketSink::init() {
        for (auto&& stream : streams()) {
//            auto avstream = avformat_new_stream(nullptr, stream->parameters->codec());
//            return_if(not_inited_ptr(avstream), Code::ERR);
//            stream->setRaw(avstream);

//            stream->setUid(utils::gen_stream_uid(uid(), stream->params->streamIndex()));
//            stream->setContext(this); //TODO
        }
        setInited(true);
        return Code::OK;
    }

    Code CustomPacketSink::open() {
        setOpened(true);
        return Code::OK;
    }

    Code CustomPacketSink::close() {
        return_if(closed(), Code::OK);
        log_debug("Closing");
        stopWait();
        try_to(stop());
        log_info("Destination \"" << _sink_name << "\" closed, " //TODO метод отрабатывает дважды: из деструктора и из онСтоп
                 << utils::time_to_string(stream(0)->params->duration(), stream(0)->params->timeBase()));
        if (stream(0)->params->duration() == 0) {
            log_warning('"' << _sink_name << "\" closed empty!");
        }
        setOpened(false);
        return Code::OK;
    }
//    Code CustomPacketSink::close() {
//        setOpened(false);
//        return Code::OK;
//    }

    std::string CustomPacketSink::toString() const {
        return "CustomPacketSink TODO";
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

    Code CustomPacketSink::onStop() {
        log_debug("onStop");
        try_to(close());
        return Code::OK;
    }

} // namespace fpp
