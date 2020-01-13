#include "CustomPacketSource.hpp"

namespace fpp {

    CustomPacketSource::CustomPacketSource(std::string source_name
                                           , StreamVector streams
                                           , std::function<Code(Packet&)> read_func
                                           , std::function<Code(Packet&)> process_func) :
        _source_name(source_name)
      , _read_func(read_func)
      , _process_func(process_func)
    {
//        setName("CustomPacketSource");
        setName("CsPktSource");
        try_throw(setStreams(streams));
    }

    CustomPacketSource::~CustomPacketSource() {
        try_throw(close());
    }

    Code CustomPacketSource::init() {
        setInited(true);
        return Code::OK;
    }

    Code CustomPacketSource::open() {
        setOpened(true);
        return Code::OK;
    }

    Code CustomPacketSource::close() {
        try_to(sendEof()); //TODO костыль?
        try_to(stop());
        stopWait(); //TODO костыль?
        log_info("Source: \"" << _source_name << "\" closed");
        setOpened(false);
        return Code::OK;
    }

    std::string CustomPacketSource::toString() const {
        return "TODO " + _source_name;
    }

    Code CustomPacketSource::readInputData(Packet& input_data) {
        try_to(_read_func(input_data));
        return Code::OK;
    }

    Code CustomPacketSource::processInputData(Packet input_data) {
        Code ret = _process_func(input_data);
        return_if(utils::exit_code(ret), ret);
        if (ret == Code::OK) {
            try_to(sendOutputData(input_data));
        }
        return ret;
    }

    Code CustomPacketSource::onStop() {
        try_to(close());
        return Code::OK;
    }

} // namespace fpp
