#include "MediaSource.hpp"

namespace fpp {

    MediaSource::MediaSource(const std::string mrl, IOType preset) :
        _input_format_context(mrl, this, preset)
    {
        setName("MediaSource");
    }

    MediaSource::~MediaSource() {
        try_throw(close());
    }

    Code MediaSource::init() {
        log_trace("Initialization.");
        try_to(_input_format_context.init());
        setInited(true);
        return Code::OK;
    }

    Code MediaSource::open() {
        log_trace("Opening.");
        return_if(opened(), Code::INVALID_CALL_ORDER);
        try_to(_input_format_context.open());
        setOpened(true);
        return Code::OK;
    }

    Code MediaSource::close() {
        log_trace("Closing.");
        return_if(closed(), Code::OK);
        try_to(sendEofPacket());
        try_to(_input_format_context.close());
        setOpened(false);
        log_info("Source: \"" << _input_format_context.mediaResourceLocator() << "\" closed.");
        return Code::OK;
    }

    std::string MediaSource::toString() const {
        /* Input #0, rtsp, from 'rtsp://admin:admin@192.168.10.3': */
        std::string str = "Input #"
                + std::to_string(_input_format_context.uid()) + ", "
                + utils::guess_format_short_name(_input_format_context.mediaResourceLocator()) + ", "
                + "from '" + _input_format_context.mediaResourceLocator() + "'";
        return str;
    }

    InputFormatContext& MediaSource::inputFormatContext() {
        return _input_format_context;
    }

    Code MediaSource::readInputData(Packet& input_data) {
        if (int ret = av_read_frame(_input_format_context.mediaFormatContext(), &input_data.raw()); ret != 0) { //TODO parse return value
            if (ret == AVERROR_EOF) {
                log_info("Source reading completed");
                return Code::END_OF_FILE;
            }
            log_error("Cannot read source: \"" << _input_format_context.mediaResourceLocator() << "\". Error " << ret);
            return Code::ERR;
        }
        //TODO этот код должен быть внутри processInputData()
        auto packet_stream = _input_format_context.stream(input_data.raw().stream_index);
        return_if(not_inited_ptr(packet_stream), Code::AGAIN);
        return_if_not(packet_stream->used(), Code::AGAIN);
        input_data.setType(packet_stream->type());
        input_data.setStreamUid(packet_stream->uid());
        return Code::OK;
    }

    Code MediaSource::processInputData(Packet input_data) {
        if (input_data.empty()) {
            try_to(sendEofPacket());
            return Code::END_OF_FILE;
//            return sendEofPacket();
        }
//        auto packet_stream = _input_format_context.stream(input_data.raw().stream_index);
//        return_if(not_inited_ptr(packet_stream), Code::AGAIN);
//        return_if_not(packet_stream->used(), Code::AGAIN);
//        input_data.setType(packet_stream->type());
//        input_data.setStreamUid(packet_stream->uid());
//        if (inited_int(_artificial_delay)) { utils::sleep_for(_artificial_delay); } //todo НЕ УДАЛЯТЬ
        return sendOutputData(input_data);
    }

    Code MediaSource::onStop() {
        log_trace("onStop");
        try_to(close());
        return Code::OK;
    }

    Code MediaSource::sendEofPacket() {
        log_trace("Sending EOF");
        Packet eof_packet;
        for (auto&& stream : _input_format_context.streams()) {
            if (stream->used()) {
                eof_packet.setType(stream->type());
                eof_packet.setStreamUid(stream->uid());
                try_to(sendOutputData(eof_packet));
                stream->setUsed(false);
            }
        }
        return Code::OK;
    }

} // namespace fpp
