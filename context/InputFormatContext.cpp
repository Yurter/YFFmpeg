#include "InputFormatContext.hpp"

namespace fpp {

    InputFormatContext::InputFormatContext(const std::string& mrl, IOPreset preset)
        : FormatContext { mrl, preset }
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

    Code InputFormatContext::seek(int64_t stream_index, int64_t timestamp, SeekPrecision seek_precision) {
        //TODO написать проверку на случай совпадения текущей метки и запрашиваемой 17.01
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
        auto ret = av_seek_frame(formatContext().get(), int(stream_index), timestamp, flags);
        return_error_if(ret != 0
                        , "Failed to seek timestamp " << utils::time_to_string(timestamp, DEFAULT_TIME_BASE) << " in stream " << stream_index
                        , Code::FFMPEG_ERROR);
        log_info("Success seek to " << utils::time_to_string(timestamp, DEFAULT_TIME_BASE));
        return Code::OK;
    }

    Code InputFormatContext::read(Packet& packet, ReadWriteMode read_mode) {
        int ret = av_read_frame(formatContext().get(), &packet.raw());

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
        setFormatContext(SharedAVFormatContext {
                             avformat_alloc_context()
                             , [](auto*& fmt_ctx){ /*if (fmt_ctx) { avformat_free_context(fmt_ctx); }*/ }
                         });
        return Code::OK;
    }

    Code InputFormatContext::openContext() {
        return_if(opened(), Code::OK);
        return_if_not(inited(), Code::INVALID_CALL_ORDER);
        if (mediaResourceLocator().find("video=") != std::string::npos) { //TODO костыль
            guessInputFromat();
        }
        return_if(mediaResourceLocator().empty(), Code::INVALID_INPUT);
        auto todo_ptr = formatContext().get();
        if (avformat_open_input(&todo_ptr, mediaResourceLocator().c_str(), _input_format, nullptr) < 0) {
            log_error("Failed to open input context.");
            return Code::INVALID_INPUT;
        }
        if (avformat_find_stream_info(formatContext().get(), nullptr) < 0) {
            log_error("Failed to retrieve input video stream information.");
            return Code::ERR;
        }
        {
            setStreams(parseFormatContext());
            return Code::OK;
        }
    }

    Code InputFormatContext::closeContext() {
        return_if(closed(), Code::OK);
        auto todo_ptr = formatContext().get();
        avformat_close_input(&todo_ptr);
//        formatContext().reset();
        return Code::OK;
    }

    StreamVector InputFormatContext::parseFormatContext() {
        StreamVector result;
        for (unsigned i = 0; i < formatContext()->nb_streams; ++i) {
            result.push_back(std::make_shared<Stream>(formatContext()->streams[i], ParamsType::Input));
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
