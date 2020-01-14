#include "CodecContext.hpp"

namespace fpp {

    CodecContext::CodecContext(const IOParams params, CodecType type) :
        params(params)
      , _codec_context(nullptr)
      , _type(type)
      , _opened(false)
    {
        setName("CodecContext");
    }

    CodecContext::~CodecContext() {
        try_throw(close());
    }

    Code CodecContext::init() {
        return_if(inited(), Code::INVALID_CALL_ORDER);
        log_debug("Initialization");
        AVCodec* codec = nullptr;
        switch (_type) { //TODO refactoring
        case CodecType::Decoder:
            codec = params.in->codec();
            break;
        case CodecType::Encoder:
            codec = params.out->codec();
            break;
        default:
            codec = nullptr;
        }
        return_if(not_inited_ptr(codec), Code::INVALID_INPUT);
        {
            _codec_context = avcodec_alloc_context3(codec);
            setName(name() + " " + codec->name);
        }
        if (not_inited_ptr(_codec_context)) {
            log_error("Failed to alloc context");
            return Code::ERR;
        }
        try_to(initParams());
        try_to(open());
        setInited(true);
        return Code::OK;
    }

    Code CodecContext::onOpen() {
        return Code::OK;
    }

    Code CodecContext::open() {
        return_if(opened(), Code::OK);
        log_debug("Opening");
        AVCodec* codec = nullptr;
        switch (_type) { //TODO refactoring
        case CodecType::Decoder:
            codec = params.in->codec();
            break;
        case CodecType::Encoder:
            codec = params.out->codec();
            break;
        default:
            codec = nullptr;
        }
        if (int ret = avcodec_open2(_codec_context, codec, nullptr); ret != 0) {
            std::string codec_type = av_codec_is_decoder(codec) ? "decoder" : "encoder";
            log_error("Cannot open codec: " << ret << ", "<< codec->name << ", " << codec_type);
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
            // использовать ? avcodec_free_context(_codec_context)
            avcodec_close(_codec_context);
        }
        setOpened(false);
        return Code::OK;
    }

    std::string CodecContext::toString() const {
        std::string str;
        std::string delimeter = ", ";
        str += "Codec name: "   + std::string(_codec_context->codec->name) + delimeter;
        str += "codec id: "     + std::to_string(_codec_context->codec->id) + delimeter;
        str += "codec type: "   + std::string(av_get_media_type_string(_codec_context->codec_type)) + delimeter;
        str += "width: "        + std::to_string(_codec_context->width) + delimeter;
        str += "height: "       + std::to_string(_codec_context->height) + delimeter;
        str += "coded_width: "  + std::to_string(_codec_context->coded_width) + delimeter;
        str += "coded_height: " + std::to_string(_codec_context->coded_height) + delimeter;
        str += "time_base: "    + utils::rational_to_string(_codec_context->time_base) + delimeter;
        str += "pix_fmt: "      + std::string(av_get_pix_fmt_name(_codec_context->pix_fmt)) + delimeter;
        str.erase(str.size() - delimeter.size(), delimeter.size());
        return str;
    }

    AVCodecContext* CodecContext::codecContext() {
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
