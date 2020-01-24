#include "InputFormatContext.hpp"
#include <core/utils.hpp>

namespace ffmpeg { extern "C" {
    #include <libavformat/avformat.h>
} } // namespace ffmpeg

namespace fpp {

    InputFormatContext::InputFormatContext(const std::string& mrl, Preset preset)
        : FormatContext { mrl, preset }
        , _input_format { nullptr } {
        setName("InpFmtCtx");
        init();
    }

    InputFormatContext::~InputFormatContext() {
        try_throw(close());
    }

    Code InputFormatContext::init() { //TODO refactoring 14.01
        switch (preset()) {
        case Auto:
            break;
        case Virtual:
            ffmpeg::avcodec_register_all();
            try_to(guessInputFromat());
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
        auto ret = av_seek_frame(context().get(), int(stream_index), timestamp, flags);
        return_error_if(ret != 0
                        , "Failed to seek timestamp " << utils::time_to_string(timestamp, DEFAULT_TIME_BASE) << " in stream " << stream_index
                        , Code::FFMPEG_ERROR);
        log_info("Success seek to " << utils::time_to_string(timestamp, DEFAULT_TIME_BASE));
        return Code::OK;
    }

    Code InputFormatContext::read(Packet& packet, ReadWriteMode read_mode) {
        const int ret = av_read_frame(context().get(), &packet.raw());

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
        auto free_context = [](auto*& fmt_ctx) { /*avformat_free_context(fmt_ctx);*/ };
        setFormatContext(SharedAVFormatContext {
            ffmpeg::avformat_alloc_context()
            , free_context
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
        auto todo_ptr = context().get();
        if (avformat_open_input(&todo_ptr, mediaResourceLocator().c_str(), inputFormat(), nullptr) < 0) {
            log_error("Failed to open input context.");
            return Code::INVALID_INPUT;
        }
        setInputFormat(context()->iformat);
        if (avformat_find_stream_info(context().get(), nullptr) < 0) {
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
        auto fmt_ctx = context().get();
        avformat_close_input(&fmt_ctx);
        log_error("after: " << fmt_ctx << " " << context().get());
//        formatContext().reset();
        return Code::OK;
    }

    StreamVector InputFormatContext::parseFormatContext() {
        StreamVector result;
        for (unsigned i = 0; i < context()->nb_streams; ++i) {
            result.push_back(std::make_shared<Stream>(context()->streams[i], ParamsType::Input));
        }
        return result;
    }

    Code InputFormatContext::guessInputFromat() {
        auto short_name = utils::guess_format_short_name(mediaResourceLocator());
        setInputFormat(ffmpeg::av_find_input_format(short_name));
        return Code::OK;
    }

    ffmpeg::AVInputFormat* InputFormatContext::inputFormat() {
        return _input_format;
    }

    void InputFormatContext::setInputFormat(ffmpeg::AVInputFormat* in_fmt) {
        _input_format = in_fmt;
    }

} // namespace fpp
