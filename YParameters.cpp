#include "YParameters.h"

YParameters::YParameters() :
    _available(false),
    _codec_id(AV_CODEC_ID_NONE),
    _codec_name(avcodec_get_name(_codec_id)),
    _bitrate(-1),
    _duration(-1),
    _stream_index(-1),
    _time_base({0,1}),
    _ignore(false)
{
    //
}

YParameters::~YParameters()
{
    //
}

void YParameters::setAvailable(bool available)
{
    _available = available;
}

void YParameters::setCodec(AVCodecID codec_id)
{
    _codec_id = codec_id;
    _codec_name = avcodec_get_name(codec_id);
}

void YParameters::setCodec(std::string codec_short_name)
{
    AVCodec *codec;
    codec = avcodec_find_decoder_by_name(codec_short_name.c_str());
    if (codec != nullptr) { _codec_id = codec->id; }
    codec = avcodec_find_encoder_by_name(codec_short_name.c_str());
    if (codec != nullptr) { _codec_id = codec->id; }
    _codec_name = codec_short_name;
}

void YParameters::setBitrate(int64_t bitrate)
{
    _bitrate = bitrate;
}

void YParameters::setDuration(int64_t duration)
{
    _duration = duration;
}

void YParameters::setStreamIndex(int64_t stream_index)
{
    _stream_index = stream_index;
}

void YParameters::setTimeBase(AVRational time_base)
{
    _time_base = time_base;
}

void YParameters::setIgnore(bool ignore)
{
    _ignore = ignore;
}

bool YParameters::available() const
{
    return _available;
}

AVCodecID YParameters::codecId() const
{
    return _codec_id;
}

std::string YParameters::codecName() const
{
    return _codec_name;
}

int64_t YParameters::bitrate() const
{
    return _bitrate;
}

int64_t YParameters::duration() const
{
    return _duration;
}

int64_t YParameters::streamIndex() const
{
    return _stream_index;
}

AVRational YParameters::timeBase() const
{
    return _time_base;
}

bool YParameters::ignore() const
{
    return _ignore;
}

void YParameters::softCopy(YParameters &other_parametrs)
{
    //
}
