#pragma once

#include "YMediaData.h"
#include "ffmpeg.h"

class YParameters : public YMediaData
{

public:

    YParameters(YMediaType type = YMediaType::MEDIA_TYPE_UNKNOWN);
    virtual ~YParameters() override = default;

    void                setCodec(AVCodecID codec_id);
    void                setCodec(std::string _codec_short_name);
    void                setCodec(AVCodec* codec);
    void                setBitrate(int64_t bitrate);
    void                setDuration(int64_t duration);
    void                setStreamIndex(int64_t stream_index);
    void                setTimeBase(AVRational time_base);

    AVCodecID           codecId()       const;
    std::string         codecName()     const;
    AVCodec*            codec()         const;
    int64_t             bitrate()       const;
    int64_t             duration()      const;
    int64_t             streamIndex()   const;
    AVRational          timeBase()      const;

    virtual std::string toString() const override;

//    virtual void        toCodecpar(AVCodecParameters* codecpar);
    virtual void        softCopy(YParameters* other_parametrs);
    YParameters&        operator=(const YParameters& rhs);

protected:

    // General
    AVCodec*            _codec;
    AVCodecID           _codec_id;
    std::string			_codec_name;
    int64_t             _bitrate;
    int64_t             _duration;
    int64_t             _stream_index;
    AVRational          _time_base;

};

