#pragma once
#include "TemplateProcessor.hpp"
#include "base/Stream.hpp"

namespace fpp {

    class Codec : public Object {

    public:

        Codec(Stream* stream, CodecType type) :
            _stream(stream)
          , _codec_context(nullptr)
          , _type(type)
        {
            setName("Codec");
        }

        virtual ~Codec() override {
            try_throw(close());
        }

        virtual Code initParams() = 0;

        virtual Code init() override {
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

        Code open() {
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
            return Code::OK;
        }
        Code close() {
            if (inited_ptr(_codec_context)) {
                // использовать ? avcodec_free_context(_codec_context)
                avcodec_close(_codec_context);
            }
            return Code::OK;
        }

        std::string toString() const override final {
            std::string str = std::string(_codec_context->codec->name)
                    + " codec";
            return str;
        }

        AVCodecContext* codecContext() {
            return _codec_context;
        }

    protected:

        Stream*             _stream;
        AVCodecContext*     _codec_context;
        CodecType           _type;

    };

} // namespace fpp
