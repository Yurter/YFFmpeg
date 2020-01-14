#include "InputFormatContext.hpp"

namespace fpp {

    InputFormatContext::InputFormatContext(const std::string mrl, IOType preset) :
        FormatContext { mrl, preset }
      , _input_format { nullptr }
    {
//        setName("InputFormatContext");
        setName("InpFmtCtx");
    }

    InputFormatContext::~InputFormatContext() {
        try_throw(close());
    }

    Code InputFormatContext::init() {
        switch (_preset) {
        case Auto:
            break;
        case Virtual:
            avdevice_register_all();
            try_to(guessInputFromat());
            _artificial_delay = 1024; //TODO: 1024 для аудио, для видео - ? (1000 / frame_rate)
//            _media_resource_locator = SINE;
            break;
        default:
            log_error("Invalid preset");
            break;
        }
        try_to(createContext());
        setInited(true);
        return Code::OK;
    }

    std::string InputFormatContext::toString() const {
        return "TODO";
    }

    Code InputFormatContext::seek(int64_t stream_index, int64_t timestamp, SeekPrecision seek_precision) {
        return_if(timestamp == av_rescale_q(stream(stream_index)->params->duration(), stream(stream_index)->params->timeBase(), DEFAULT_TIME_BASE), Code::OK); //TODO кривой код
        int flags = 0;
        switch (seek_precision) {
        case SeekPrecision::Forward:
            flags = 0;
            break;
        case SeekPrecision::Backward:
            flags = AVSEEK_FLAG_BACKWARD;
            break;
        case SeekPrecision::Any:
            flags = AVSEEK_FLAG_ANY;
            break;
        case SeekPrecision::Precisely:
            return Code::NOT_IMPLEMENTED;
        }
        auto ret = av_seek_frame(_format_context, int(stream_index), timestamp, flags);
        return_error_if(ret != 0
                        , "Failed to seek timestamp " << utils::time_to_string(timestamp, DEFAULT_TIME_BASE) << " in stream " << stream_index
                        , Code::FFMPEG_ERROR);
        log_info("Success seek to " << utils::time_to_string(timestamp, DEFAULT_TIME_BASE));
        return Code::OK;
    }

//    Code InputFormatContext::read(Packet& packet, ReadWriteMode read_mode) {
//        switch (read_mode) {
//        case ReadWriteMode::Instant: {
//            int ret = av_read_frame(mediaFormatContext(), &packet.raw());
//            return_info_if(ret == AVERROR_EOF
//                           , "Reading completed"
//                           , Code::END_OF_FILE);
//            return_error_if(ret < 0
//                            , "Cannot read source: \"" << mediaResourceLocator() << "\". "
//                            , Code::FFMPEG_ERROR);
//            break;
//        }
//        case ReadWriteMode::Interleaved:

//            return Code::NOT_IMPLEMENTED;
//        }
//        initPacket(packet);
//        return Code::OK;
//    }
    Code InputFormatContext::read(Packet& packet, ReadWriteMode read_mode) {
        int ret = av_read_frame(mediaFormatContext(), &packet.raw());

        return_info_if(ret == AVERROR_EOF
                       , "Reading completed"
                       , Code::END_OF_FILE);

        return_error_if(ret < 0
                        , "Cannot read source: \"" << mediaResourceLocator() << "\". "
                        , Code::FFMPEG_ERROR);

        if (read_mode == ReadWriteMode::Interleaved) { //TODO 13.01
            utils::sleep_for_ms(23); //TODO сделать задержку (1024мс в tb{1,1000}) 13.01
        }

        initPacket(packet);
        return Code::OK;
    }

    Code fpp::InputFormatContext::createContext() {
        return_error_if(inited_ptr(_format_context)
                        , "Context already created!"
                        , Code::INVALID_CALL_ORDER);
        _format_context = avformat_alloc_context();
        if (not_inited_ptr(_format_context)) {
            log_error("Failed to alloc input context.");
            return Code::ERR;
        }
        return Code::OK;
    }

    Code InputFormatContext::openContext() {
        return_if_not(inited(), Code::INVALID_CALL_ORDER);
        if (_media_resource_locator.find("video=") != std::string::npos) { //TODO костыль
            guessInputFromat();
        }
        return_if(_media_resource_locator.empty(), Code::INVALID_INPUT);
        for (auto&& avstream : streams()) {
            try_to(avstream->init());
        }
        if (avformat_open_input(&_format_context, _media_resource_locator.c_str(), _input_format, nullptr) < 0) {
            log_error("Failed to open input context.");
            return Code::INVALID_INPUT;
        }
        if (avformat_find_stream_info(_format_context, nullptr) < 0) {
            log_error("Failed to retrieve input video stream information.");
            return Code::ERR;
        }
        {
            _input_format = _format_context->iformat;
            try_to(parseFormatContext());
            av_dump_format(_format_context, 0, _media_resource_locator.c_str(), 0);
            return Code::OK;
        }
    }

    Code InputFormatContext::closeContext() {
        return_if(closed(), Code::OK);
        if (inited_ptr(_format_context)) {
            avformat_close_input(&_format_context);
            return Code::OK;
        }
        return Code::INVALID_CALL_ORDER;
    }

    Code InputFormatContext::guessInputFromat() {
        auto short_name = utils::guess_format_short_name(_media_resource_locator);
        AVInputFormat* input_format = av_find_input_format(short_name.c_str());
        if (input_format == nullptr) {
            log_error("Failed guess input format: " << _media_resource_locator);
            return Code::INVALID_INPUT;
        }
        _input_format = input_format;
        return Code::OK;
    }

    void InputFormatContext::initPacket(Packet& packet) {
        auto packet_stream = stream(packet.streamIndex());
        packet.setType(packet_stream->type());
//        packet.setStreamIndex(packet_stream->index()); //TODO надо ли ставить тут? 10.01
//        packet.setStreamUid(packet_stream->params->streamUid());
        packet.setTimeBase(packet_stream->params->timeBase());
    }

} // namespace fpp
