#include "YAbstractCodec.h"

YAbstractCodec::YAbstractCodec(YStream* stream) :
    _stream(stream),
    _codec_context(nullptr)
{
    setName("YAbstractCodec");
}

YAbstractCodec::~YAbstractCodec()
{
    avcodec_close(_codec_context);
}

YCode YAbstractCodec::init()
{
    AVCodec* codec = findCodec();
    return_if(not_inited_ptr(codec), YCode::INVALID_INPUT);
    {
        _codec_context = avcodec_alloc_context3(codec);
        setName(name() + " " + codec->name);
    }
    if (not_inited_ptr(_codec_context)) {
        log_error("Failed to alloc context");
        return YCode::ERR;
    }
    utils::parameters_to_avcodecpar(_stream->parameters, _stream->codecParameters());
    if (avcodec_parameters_to_context(_codec_context, _stream->codecParameters()) < 0) {
        log_error("avcodec_parameters_to_context failed");
        return YCode::ERR;
    }
    utils::parameters_to_context(_stream->parameters, _codec_context);
    if (avcodec_open2(_codec_context, codec, nullptr) < 0) {
        log_error("Cannot open codec");
        return YCode::ERR;
    }
    { /* Crutch */ //TODO
        if (_stream->isAudio()) {
            _codec_context->channel_layout = static_cast<uint64_t>(
                av_get_default_channel_layout(_codec_context->channels)
            );
        }
    }
    return YCode::OK;
}

AVCodecContext* YAbstractCodec::codecContext()
{
    return _codec_context;
}
