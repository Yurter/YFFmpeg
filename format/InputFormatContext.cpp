#include "InputFormatContext.hpp"

namespace fpp {

    InputFormatContext::InputFormatContext(const std::string& mrl, StreamSetter stream_setter, IOPreset preset)
        : FormatContext { mrl, stream_setter, preset }
        , _input_format { nullptr } {
        setName("InpFmtCtx");
    }

    InputFormatContext::~InputFormatContext() {
        try_throw(close());
    }

    Code InputFormatContext::init() { //TODO refactoring 14.01
        switch (preset()) {
        case Auto:
            break;
        case Virtual:
            avdevice_register_all();
            try_to(guessInputFromat());
//            _artificial_delay = 1024; //TODO: 1024 для аудио, для видео - ? (1000 / frame_rate)
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
//        return_if(timestamp == av_rescale_q(stream(stream_index)->params->duration(), stream(stream_index)->params->timeBase(), DEFAULT_TIME_BASE), Code::OK); //TODO кривой код
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
        auto ret = av_seek_frame(mediaFormatContext(), int(stream_index), timestamp, flags);
        return_error_if(ret != 0
                        , "Failed to seek timestamp " << utils::time_to_string(timestamp, DEFAULT_TIME_BASE) << " in stream " << stream_index
                        , Code::FFMPEG_ERROR);
        log_info("Success seek to " << utils::time_to_string(timestamp, DEFAULT_TIME_BASE));
        return Code::OK;
    }

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

        return Code::OK;
    }

    Code fpp::InputFormatContext::createContext() {
        return_error_if(inited_ptr(mediaFormatContext())
                        , "Context already created!"
                        , Code::INVALID_CALL_ORDER);
        try_to(setMediaFormatContext(avformat_alloc_context()));
        return_error_if(not_inited_ptr(mediaFormatContext())
                        , "Failed to alloc input context."
                        , Code::ERR);
        return Code::OK;
    }

    Code InputFormatContext::openContext() {
        return_if(opened(), Code::OK);
        return_if_not(inited(), Code::INVALID_CALL_ORDER);
        if (mediaResourceLocator().find("video=") != std::string::npos) { //TODO костыль
            guessInputFromat();
        }
        return_if(mediaResourceLocator().empty(), Code::INVALID_INPUT);
//        for (auto&& avstream : streams()) { //TODO роверить необходимость инициализации 15.01
//            try_to(avstream->init());
//        }
        if (avformat_open_input(mediaFormatContext2(), mediaResourceLocator().c_str(), _input_format, nullptr) < 0) {
            log_error("Failed to open input context.");
            return Code::INVALID_INPUT;
        }
        if (avformat_find_stream_info(mediaFormatContext(), nullptr) < 0) {
            log_error("Failed to retrieve input video stream information.");
            return Code::ERR;
        }
        {
            auto input_streams = parseFormatContext();
            try_to(registerStreams(input_streams));
            return Code::OK;
        }
    }

    Code InputFormatContext::closeContext() {
        return_if(closed(), Code::OK);
        avformat_close_input(mediaFormatContext2());
        return Code::OK;
    }

    StreamVector InputFormatContext::parseFormatContext() {
        StreamVector result;
        for (unsigned i = 0; i < mediaFormatContext()->nb_streams; ++i) {
            result.push_back(std::make_shared<Stream>(mediaFormatContext()->streams[i]));
//            result.back()->params->setCodecType(CodecType::Decoder); //TODO crutch  16.01
        }
        return result;
    }

    Code InputFormatContext::guessInputFromat() {
        auto short_name = utils::guess_format_short_name(mediaResourceLocator());
        AVInputFormat* input_format = av_find_input_format(short_name.c_str());
        if (input_format == nullptr) {
            log_error("Failed guess input format: " << mediaResourceLocator());
            return Code::INVALID_INPUT;
        }
        _input_format = input_format;
        return Code::OK;
    }

} // namespace fpp
