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
    AVCodec *codec;
    auto stream_index = av_find_best_stream(_stream->mediaContext()->mediaFormatContext()
                                            , utils::ymedia_type_to_avmedia_type(_stream->type())
                                            , -1, -1, &codec, 0);
    if (stream_index < 0) {
        log_error("Cannot find an av stream in the input file");
        return YCode::INVALID_INPUT;
    }
    {
        _codec_context = avcodec_alloc_context3(codec);
        setName(name() + " " + codec->name);
    }
    if (!_codec_context) {
        log_error("Failed to alloc context");
        return YCode::ERR;
    }
    if (avcodec_parameters_to_context(_codec_context, _stream->codecParameters()) < 0) {
        log_error("avcodec_parameters_to_context failed");
        return YCode::ERR;
    }
    if (avcodec_open2(_codec_context, codec, nullptr) < 0) {
        log_error("Cannot open av codec");
        return YCode::ERR;
    }
    {
        /* Crutch */ //TODO
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
