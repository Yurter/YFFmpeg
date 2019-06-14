#include "YParameters.h"

YParameters::YParameters() :
    _codec_id(DEFAULT_CODEC_ID),
    _codec_name(DEFAULT_STRING),
    _bitrate(DEFAULT_FLOAT),
    _duration(DEFAULT_INT),
    _stream_index(INVALID_INT),
    _time_base(DEFAULT_RATIONAL)
{
    //
}

YParameters::~YParameters()
{
    //
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

void YParameters::softCopy(YParameters& other_parametrs)
{
    if (not_inited_codec_id(_codec_id)) { _codec_id = other_parametrs.codecId();            }
    if (not_inited_string(_codec_name)) { _codec_name = other_parametrs.codecName();        }
    if (not_inited_int(_bitrate))       { _bitrate = other_parametrs.bitrate();             }
    if (not_inited_int(_duration))      { _duration = other_parametrs.duration();           }
    if (invalid_int(_stream_index))     { _stream_index = other_parametrs.streamIndex();    }
    if (not_inited_q(_time_base))       { _time_base = other_parametrs.timeBase();          }
}

YParameters &YParameters::operator=(const YParameters &rhs)
{
    _codec_id =     rhs.codecId();
    _codec_name =   rhs.codecName();
    _bitrate =      rhs.bitrate();
    _duration =     rhs.duration();
    _stream_index = rhs.streamIndex();
    _time_base =    rhs.timeBase();
    return *this;
}
