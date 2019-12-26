#include "MediaSource.hpp"

namespace fpp {

    MediaSource::MediaSource(const std::string mrl, IOType preset) :
        _input_format_context(mrl, preset)
      , _start_time_point(FROM_START)
      , _end_time_point(TO_END)
    {
        _doNotSendEOF = false;
        setName("MediaSource");
        setDiscardType(MediaType::MEDIA_TYPE_AUDIO);
    }

    MediaSource::~MediaSource() {
        try_throw(close());
    }

    Code MediaSource::init() {
        return_if(inited(), Code::OK);
        log_debug("Initialization");
        try_to(_input_format_context.init());
        setInited(true);
        return Code::OK;
    }

    Code MediaSource::open() {
        return_if(opened(), Code::OK);
        log_debug("Opening");
        log_info('"' << _input_format_context.mediaResourceLocator() << "\" is opening...");
        try_to(_input_format_context.open());
        log_info('"' << _input_format_context.mediaResourceLocator() << "\" opened");
        { //TODO refactoring
            try_to(setStreams(_input_format_context.streams()));
            for (auto&& avstream : streams()) { //TODO
                try_to(avstream->init());
            }
            if (_start_time_point != FROM_START) {
                for (auto& stream : streams()) {
                    try_to(_input_format_context.seek(stream->index(), _start_time_point));
                }
            }
        }
        setOpened(true);
        return Code::OK;
    }

    Code MediaSource::close() {
        return_if(closed(), Code::OK);
        log_debug("Closing");
//        try_to(sendEofPacket()); //TODO костыль?
        try_to(sendEof()); //TODO костыль?
        try_to(stop());
        stopWait(); //TODO костыль?
        try_to(_input_format_context.close());
        log_info("Source \"" << _input_format_context.mediaResourceLocator() << "\" closed, "
                 << utils::msec_to_time(stream(0)->params->duration()));
        setOpened(false);
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

    bool MediaSource::equalTo(const ProcessorPointer other) const {
        auto other_media_source = dynamic_cast<const MediaSource * const>(other.get());
        return_if(not_inited_ptr(other_media_source), false);

        return_if(this->inputFormatContext()->mediaResourceLocator()
                  == other_media_source->inputFormatContext()->mediaResourceLocator(), true);

        return false;
    }

    void MediaSource::setStartPoint(int64_t value) {
        return_if(_start_time_point == value, void());
        if ((value != FROM_START) && (value < 0)) {
            log_warning("Cannot set start time point less then zero: " << value << ", ignored");
            return;
        }
        if ((_end_time_point != TO_END) && (value > _end_time_point)) {
            log_warning("Cannot set start time point more then end time point "
                        << _end_time_point <<  ": " << value << ", ignored");
            return;
        }
        _start_time_point = value;
    }

    void MediaSource::setEndPoint(int64_t value) {
        return_if(_end_time_point == value, void());
        if ((value != TO_END) && (value < 0)) {
            log_warning("Cannot set end time point less then zero: " << value << ", ignored");
            return;
        }
        if ((_start_time_point != FROM_START) && (value < _start_time_point)) {
            log_warning("Cannot set end time point less then start time point "
                        << _start_time_point <<  ": " << value << ", ignored");
            return;
        }
        _end_time_point = value;
    }

    const InputFormatContext * MediaSource::inputFormatContext() const {
        return &_input_format_context;
    }

    Code MediaSource::readInputData(Packet& input_data) {
        if (_end_time_point != TO_END) { //TODO
            const auto planned_duration = _end_time_point - _start_time_point;
            const auto actual_duration = stream(input_data.streamIndex())->params->duration();
            return_if(actual_duration >= planned_duration, Code::END_OF_FILE);
        }

        try_to(_input_format_context.read(input_data));
        return Code::OK;
    }

    Code MediaSource::processInputData(Packet input_data) {
        if (stream(input_data.streamIndex())->packetIndex() == 0) { //TODO костыль? каждый раз проверяет.. -> перенести в онСтарт - раз прочитать пакеты из каждого потока
            determineStampType(input_data);
        }

        try_to(stream(input_data.streamIndex())->stampPacket(input_data));

        if (inputDataCount() == 0) { //TODO нуженл ли этот лог?
            log_debug("Read from "
                      << (_start_time_point == FROM_START ? "start" : utils::msec_to_time(_start_time_point))
                      << ", first packet: " << input_data);
        }

        log_warning("IN : " << input_data);

        return sendOutputData(input_data);
    }

    Code MediaSource::onStop() {
        log_debug("onStop");
//        try_to(close());
        return Code::OK;
    }

//    Code MediaSource::sendEofPacket() {
//        return_if(_doNotSendEOF, Code::OK);
//        log_debug("Sending EOF");
//        Packet eof_packet;
//        for (auto&& stream : _input_format_context.streams()) {
//            if (stream->used()) {
//                eof_packet.setType(stream->type());
//                eof_packet.setStreamUid(stream->params->streamUid());
//                try_to(sendOutputData(eof_packet));
//                stream->setUsed(false);
//            }
//        }
//        return Code::OK;
//    }

    void MediaSource::determineStampType(const Packet& packet) {
        if (packet.pts() != 0) { /* Требуется перештамповывать пакеты */
            if (_start_time_point == FROM_START) { /* Чтение из источника, передающего пакеты не с начала */
                stream(packet.streamIndex())->setStampType(StampType::Realtime);
            } else {
                stream(packet.streamIndex())->setStampType(StampType::Offset); /* Происходит чтение не с начала файла */
            }
        } else {
//            if (stream(packet.streamIndex())->params->timeBase() != DEFAULT_TIME_BASE) { /* Требуется рескейлить в миллисекунды */
//                stream(packet.streamIndex())->setStampType(StampType::Rescale);
//            } else {
//                stream(packet.streamIndex())->setStampType(StampType::Copy);
//            }
            stream(packet.streamIndex())->setStampType(StampType::Copy);
        }
    }

} // namespace fpp
