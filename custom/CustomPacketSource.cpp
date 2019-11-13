#include "CustomPacketSource.hpp"

namespace fpp {

    CustomPacketSource::CustomPacketSource(std::string source_name
                                           , StreamVector streams
                                           , std::function<Code(Packet&)> read_func
                                           , std::function<Code(Packet&)> process_func) :
        _source_name(source_name)
      , _streams(streams)
      , _read_func(read_func)
      , _process_func(process_func)
    {
        setName("CustomPacketSource");
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
        try_to(sendEofPacket());
        setOpened(false);
        log_info("Source: \"" << _source_name << "\" closed.");
        return Code::OK;
    }

    std::string CustomPacketSource::toString() const {
        return "TODO " + _source_name;
    }

    StreamVector CustomPacketSource::streams() {
        return _streams;
    }

    Stream* CustomPacketSource::stream(int64_t index) {
        if (size_t(index) < _streams.size()) {
            return _streams[size_t(index)];
        } else {
            return nullptr;
        }
    }

    Stream* CustomPacketSource::stream(MediaType type) {
        for (auto str : _streams) {
            if (str->typeIs(type)) {
                return str;
            }
        }
        return nullptr;
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

    Code CustomPacketSource::sendEofPacket() {
        Packet eof_packet;
        for (auto&& stream : _streams) {
            if (stream->used()) {
                eof_packet.setType(stream->type());
                eof_packet.setStreamUid(stream->uid());
                try_to(sendOutputData(eof_packet));
                stream->setUsed(false);
            }
        }
        return Code::END_OF_FILE;
    }

} // namespace fpp
