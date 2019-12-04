#include "MediaSource.hpp"

namespace fpp {

    MediaSource::MediaSource(const std::string mrl, IOType preset) :
        _input_format_context(mrl, this, preset)
      , _start_point(FROM_START)
      , _end_point(TO_END)
    {
        _doNotSendEOF = false;
        setName("MediaSource");
    }

    MediaSource::~MediaSource() {
        try_throw(close());
    }

    Code MediaSource::init() {
        log_debug("Initialization.");
        return_if(inited(), Code::INVALID_CALL_ORDER);
        try_to(_input_format_context.init());
        setInited(true);
        return Code::OK;
    }

    Code MediaSource::open() {
        log_debug("Opening.");
        return_if(opened(), Code::INVALID_CALL_ORDER);
        log_info(_input_format_context.mediaResourceLocator() << "\" is opening...");
        try_to(_input_format_context.open());
        log_info(_input_format_context.mediaResourceLocator() << "\" opened.");
        { //TODO refactoring
            try_to(setStreams(_input_format_context.streams()));
            for (auto&& avstream : streams()) { //TODO
                try_to(avstream->init());
            }
            if (_start_point != FROM_START) {
                for (auto& stream : streams()) {
                    try_to(_input_format_context.seek(stream->index(), _start_point));
                }
            }
        }
        setOpened(true);
        return Code::OK;
    }

    Code MediaSource::close() {
        log_debug("Closing.");
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

    bool MediaSource::equalTo(const Processor * const other) const {
        auto other_media_source = dynamic_cast<const MediaSource * const>(other);
        return_if(not_inited_ptr(other_media_source), false);

        return_if(this->inputFormatContext()->mediaResourceLocator()
                  == other_media_source->inputFormatContext()->mediaResourceLocator(), true);
        return false;
    }

    void MediaSource::setStartPoint(int64_t value) {
        _start_point = value;
    }

    void MediaSource::setEndPoint(int64_t value) {
        _end_point = value;
    }

    const InputFormatContext * MediaSource::inputFormatContext() const {
        return &_input_format_context;
    }

    Code MediaSource::readInputData(Packet& input_data) {
        if (_end_point != -1) { //TODO
            if (inited_ptr(stream(0))) {
                if (stream(0)->parameters->duration() > (_end_point - _start_point)) {
                    return Code::END_OF_FILE;
                }
            }
        }
        if (int ret = av_read_frame(_input_format_context.mediaFormatContext(), &input_data.raw()); ret != 0) {
            return_info_if(ret == AVERROR_EOF
                           , "Source reading completed."
                           , Code::END_OF_FILE);
            log_error("Cannot read source: \"" << _input_format_context.mediaResourceLocator() << "\". Error " << ret);
            return Code::FFMPEG_ERROR;
        }
        if (input_data.streamIndex() != 0) {
            return Code::AGAIN; //TODO fix it
        }
        //TODO этот код должен быть внутри processInputData()
        auto packet_stream = _input_format_context.stream(input_data.raw().stream_index);
        return_if(not_inited_ptr(packet_stream), Code::AGAIN);
        return_if_not(packet_stream->used(), Code::AGAIN);
        input_data.setType(packet_stream->type());
        input_data.setStreamUid(packet_stream->uid());
//        stream(0)->parameters->increaseDuration(input_data.duration()); //TODO
        stream(0)->parameters->increaseDuration(stream(0)->packet_duration()); //TODO
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
        log_debug("onStop");
        try_to(close());
        return Code::OK;
    }

    Code MediaSource::sendEofPacket() {
        return_if(_doNotSendEOF, Code::OK);
        log_debug("Sending EOF");
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
