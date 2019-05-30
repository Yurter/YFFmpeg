#include "YAbstractCoder.h"

YAbstractCoder::YAbstractCoder() :
    _video_stream_index(-1),
    _audio_stream_index(-1),
    _video_codec_context(nullptr),
    _audio_codec_context(nullptr)
{
    //
}

YAbstractCoder::~YAbstractCoder()
{
    avcodec_close(_video_codec_context);
    avcodec_close(_audio_codec_context);
}

AVCodecContext* YAbstractCoder::videoCodecContext()
{
    return _video_codec_context;
}

AVCodecContext* YAbstractCoder::audioCodecContext()
{
    return _audio_codec_context;
}

bool YAbstractCoder::compatibleWithSampleFormat(AVSampleFormat sample_format)
{
    auto smp_fmt = _audio_codec_context->codec->sample_fmts;
    while (smp_fmt[0] != AV_SAMPLE_FMT_NONE) {
        if (smp_fmt[0] == sample_format) { return true; }
        smp_fmt++;
    }
    return false;
}
