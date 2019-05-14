#include "YAbstractCoder.h"

YAbstractCoder::YAbstractCoder() :
    _video_codec_context(nullptr),
    _audio_codec_context(nullptr)
{
    //
}

YAbstractCoder::~YAbstractCoder()
{
    // TODO
}

AVCodecContext* YAbstractCoder::videoCodecContext()
{
    return _video_codec_context;
}

AVCodecContext* YAbstractCoder::audioCodecContext()
{
    return _audio_codec_context;
}
