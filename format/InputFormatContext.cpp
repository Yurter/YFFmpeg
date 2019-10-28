#include "InputFormatContext.hpp"

namespace fpp {

    InputFormatContext::InputFormatContext(const std::string mrl, IOType preset) :
        FormatContext(mrl, preset)
    {
        setName("InputFormatContext");
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

    Code fpp::InputFormatContext::createContext() {
        _format_context = avformat_alloc_context();
        if (not_inited_ptr(_format_context)) {
            log_error("Failed to alloc input context.");
            return Code::ERR;
        }
        return Code::OK;
    }

    Code InputFormatContext::openContext() {
        log_info("MediaSource: \"" << _media_resource_locator << "\" is opening...");
        return_if(_media_resource_locator.empty(), Code::INVALID_INPUT);
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
            log_info("MediaSource: \"" << _media_resource_locator << "\" opened.");
            return Code::OK;
        }
    }

    Code InputFormatContext::closeContext() {
        if (inited_ptr(_format_context)) {
            avformat_close_input(&_format_context);
            return Code::OK;
        }
        return Code::INVALID_CALL_ORDER;
    }

    Code InputFormatContext::guessInputFromat(){
        auto short_name = utils::guess_format_short_name(_media_resource_locator);
        AVInputFormat* input_format = av_find_input_format(short_name.c_str());
        if (input_format == nullptr) {
            log_error("Failed guess input format: " << _media_resource_locator);
            return Code::INVALID_INPUT;
        }
        _input_format = input_format;
        return Code::OK;
    }

} // namespace fpp
