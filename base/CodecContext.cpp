#include "CodecContext.hpp"
#include <core/Utils.hpp>

namespace fpp {

    CodecContext::CodecContext(const IOParams params)
        : params { params }
        , _codec_context { nullptr }
        , _opened { false } {
        setName("CodecContext");
        open();
    }

    CodecContext::~CodecContext() {
        close();
    }

    void CodecContext::init() {
//        return_if(inited(), Code::INVALID_CALL_ORDER);
        log_debug("Initialization");
        _codec_context.reset(
            avcodec_alloc_context3(codec())
            , [](ffmpeg::AVCodecContext*& codec_context) { avcodec_free_context(&codec_context); }
        );
        if (!_codec_context) {
            throw FFmpegException("avcodec_alloc_context3 failed");
        }
        setName(name() + " " + codec()->name);
        utils::parameters_to_context(parameters(), _codec_context.get());
        if (true) { // TODO костыль
            _codec_context->time_base = params.in->timeBase();
        }
        /*try_to(*/open()/*)*/;
        setInited(true);
//        return Code::OK;
    }

    Code CodecContext::open() {
        return_if(opened(), Code::OK);
        log_debug("Opening");
        if (int ret = avcodec_open2(_codec_context.get(), codec(), nullptr); ret != 0) {
            const std::string codec_type = av_codec_is_decoder(codec()) ? "decoder" : "encoder";
            log_error("Cannot open codec: " << ret << ", "<< codec()->name << ", " << codec_type);
            return Code::ERR;
        }
        try_to(onOpen());
        setOpened(true);
        return Code::OK;
    }

    Code CodecContext::close() {
        return_if(closed(), Code::OK);
        log_debug("Closing");
        if (inited_ptr(_codec_context)) {
            avcodec_close(_codec_context.get());
        }
        setOpened(false);
        return Code::OK;
    }

    std::string CodecContext::toString() const {
        std::string str;
        const std::string delimeter = ", ";
        str += "Codec name: "   + std::string(_codec_context->codec->name)      + delimeter;
        str += "codec id: "     + std::to_string(_codec_context->codec->id)     + delimeter;
        str += "codec type: "   + utils::to_string(_codec_context->codec_type)  + delimeter;
        str += "width: "        + std::to_string(_codec_context->width)         + delimeter;
        str += "height: "       + std::to_string(_codec_context->height)        + delimeter;
        str += "coded_width: "  + std::to_string(_codec_context->coded_width)   + delimeter;
        str += "coded_height: " + std::to_string(_codec_context->coded_height)  + delimeter;
        str += "time_base: "    + utils::to_string(_codec_context->time_base)   + delimeter;
        str += "pix_fmt: "      + utils::to_string(_codec_context->pix_fmt)     + delimeter;
        str.erase(str.size() - delimeter.size(), delimeter.size());
        return str;
    }

    SharedAVCodecContext CodecContext::context() {
        return _codec_context;
    }

    bool CodecContext::opened() const {
        return _opened;
    }

    bool CodecContext::closed() const {
        return !_opened;
    }

    void CodecContext::setOpened(bool value) {
        _opened = value;
    }

} // namespace fpp
