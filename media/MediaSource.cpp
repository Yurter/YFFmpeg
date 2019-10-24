#include "MediaSource.hpp"

namespace fpp {

    MediaSource::MediaSource(const std::string mrl, IOType preset) :
        FormatContext(mrl, preset)
        , _input_format(nullptr)
    {
        setName("MediaSource");
//        try_throw(setInOutFunction(std::bind(&MediaSource::readPacket, this)));
    }

    MediaSource::~MediaSource() {
        try_throw(close());
    }

    Code MediaSource::init() {
        switch (_preset) {
        case Auto:
            break;
        case Virtual:
            avdevice_register_all();
            try_to(guessInputFromat());
            _artificial_delay = 1024; //TODO: 1024 для аудио, для видео - ? (1000 / frame_rate)
            break;
        default:
            log_error("Invalid preset");
            break;
        }
        try_to(createContext());
        setInited(true);
        return Code::OK;
    }

    Code MediaSource::open() {
        return_if(opened(), Code::INVALID_CALL_ORDER);
        try_to(openContext());
        return Code::OK;
    }

    Code MediaSource::close() {
        try_to(sendEofPacket());
        return_if(closed(), Code::INVALID_CALL_ORDER);
        if (_format_context != nullptr) { avformat_close_input(&_format_context); }
        return_if_not(closed(), Code::ERR);
        log_info("Source: \"" << _media_resource_locator << "\" closed.");
        return Code::OK;
    }

    std::string MediaSource::toString() const {
        /* Input #0, rtsp, from 'rtsp://admin:admin@192.168.10.3': */
        std::string str = "Input #"
                + std::to_string(uid()) + ", "
                + utils::guess_format_short_name(_media_resource_locator) + ", "
                + "from '" + _media_resource_locator + "'";
        return str;
    }

    Code MediaSource::readInputData(Packet& input_data) {
        if (int ret = av_read_frame(mediaFormatContext(), &input_data.raw()); ret != 0) { //TODO parse return value
            if (ret == AVERROR_EOF) {
                log_info("Source reading completed");
                return Code::END_OF_FILE;
            }
            log_error("Cannot read source: \"" << _media_resource_locator << "\". Error " << ret);
            return Code::ERR;
        }
        return Code::OK;
    }

    Code MediaSource::processInputData(Packet input_data) {
        if (input_data.empty()) {
            return sendEofPacket();
        }
        auto packet_stream = stream(input_data.raw().stream_index);
        return_if(not_inited_ptr(packet_stream), Code::AGAIN);
        return_if_not(packet_stream->used(), Code::AGAIN);
        input_data.setType(packet_stream->type());
        input_data.setStreamUid(packet_stream->uid());
        if (inited_int(_artificial_delay)) { utils::sleep_for(_artificial_delay); } //todo
        return sendOutputData(input_data);
    }

    Code MediaSource::guessInputFromat() {
        auto shorn_name = utils::guess_format_short_name(_media_resource_locator);
        AVInputFormat* input_format = av_find_input_format(shorn_name.c_str());
        if (input_format == nullptr) {
            log_error("Failed guess input format: " << _media_resource_locator);
            return Code::INVALID_INPUT;
        }
        _input_format = input_format;
        return Code::OK;
    }

    Code MediaSource::sendEofPacket() {
        Packet eof_packet;
        for (auto&& stream : streams()) {
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
