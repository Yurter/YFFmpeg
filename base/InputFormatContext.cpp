#include "InputFormatContext.hpp"

namespace fpp {

    InputFormatContext::InputFormatContext() {
        //
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
            setInited(true);
            setOpened(true);
            return Code::OK;
        }
    }

} // namespace fpp
