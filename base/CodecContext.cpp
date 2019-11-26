#include "CodecContext.hpp"

namespace fpp {

    CodecContext::CodecContext(Stream* stream, CodecType type) :
        _stream(stream)
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
        log_trace("Initialization.");
        return_if(inited(), Code::INVALID_CALL_ORDER);
        auto codec = _stream->parameters->codec();
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

    Code CodecContext::open() {
        log_trace("Opening.");
        return_if(opened(), Code::INVALID_CALL_ORDER);
        auto codec = _stream->parameters->codec();
        if (int ret = avcodec_open2(_codec_context, codec, nullptr); ret != 0) {
            std::string codec_type = av_codec_is_decoder(codec) ? "decoder" : "encoder";
            log_error("Cannot open codec: " << ret << ", "<< codec->name << ", " << codec_type);
            return Code::ERR;
        }
        { /* Crutch */ //TODO
            if (_stream->parameters->isAudio()) {
                _codec_context->channel_layout = static_cast<uint64_t>(
                            av_get_default_channel_layout(_codec_context->channels)
                            );
            }
        }
        setOpened(true);
        return Code::OK;
    }

    Code CodecContext::close() {
        log_trace("Closing.");
        return_if(closed(), Code::INVALID_CALL_ORDER);
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

        //debug output
        str += "object size: "  + std::to_string(sizeof (*static_cast<Parameters*>(_stream->parameters))) + delimeter;
        str += "object raw: \n";
        for (size_t i = 0; i < sizeof (*static_cast<Parameters*>(_stream->parameters)); ++i) {
            if (i % 16 == 0) {
                str += "\n";
            }
            std::stringstream stream;
            auto number = unsigned(reinterpret_cast<const uint8_t*>(_stream->parameters)[i]);
            stream << std::hex << number;
            if (stream.str().length() == 1) {
                str += "0" + stream.str();
            } else {
                str += stream.str();
            }
            str += " ";
        }
        str += "object enum: "  + std::to_string(sizeof (MediaData)) + delimeter;
        str += "data: " + std::to_string(static_cast<MediaData*>(_stream->parameters)->type()) + delimeter;
        str += "duration: " + std::to_string(_stream->parameters->duration()) + delimeter;
        str += "___";
        //~debug output

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
