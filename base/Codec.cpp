#include "Codec.hpp"

namespace fpp {

    Codec::Codec(fpp::Stream *stream, CodecType type) :
        _stream(stream)
      , _codec_context(nullptr)
      , _type(type)
    {
        setName(utils::codec_type_to_string(_type));
    }

    fpp::Codec::~Codec() {
        close();
    }

    Code fpp::Codec::init() {
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
        switch (_type) {
        case CodecType::Decoder: /* Инициализация параметров контекста декодера */
            utils::parameters_to_avcodecpar(_stream->parameters, _stream->codecParameters());
            if (avcodec_parameters_to_context(_codec_context, _stream->codecParameters()) < 0) {
                log_error("avcodec_parameters_to_context failed");
                return Code::ERR;
            }
            utils::parameters_to_context(_stream->parameters, _codec_context);
            break;
        case CodecType::Encoder: /* Инициализация параметров контекста энкодера */
            utils::parameters_to_context(_stream->parameters, _codec_context);
            utils::parameters_to_avcodecpar(_stream->parameters, _stream->codecParameters());
            if (avcodec_parameters_to_context(_codec_context, _stream->codecParameters()) < 0) {
                log_error("avcodec_parameters_to_context failed");
                return Code::ERR;
            }
            break;
        }
        open();
        setInited(true);
        return Code::OK;
    }

    Code Codec::open() {
        auto codec = _stream->parameters->codec();
        if (int ret = avcodec_open2(_codec_context, codec, nullptr); ret != 0) {
            std::string codec_type = av_codec_is_decoder(codec) ? "decoder" : "encoder";
            log_error("Cannot open codec: " << ret << ", "<< codec->name << ", " << codec_type);
            return Code::ERR;
        }
        { /* Crutch */ //TODO
            if (_stream->isAudio()) {
                _codec_context->channel_layout = static_cast<uint64_t>(
                            av_get_default_channel_layout(_codec_context->channels)
                            );
            }
        }
    }

    Code Codec::close() {
        if (inited_ptr(_codec_context)) {
            avcodec_close(_codec_context);
        }
    }

    std::string fpp::Codec::toString() const {
        std::string str = std::string(_codec_context->codec->name)
                + " codec";
        return str;
    }

    AVCodecContext *fpp::Codec::codecContext() {
        return _codec_context;
    }

} // namespace fpp
