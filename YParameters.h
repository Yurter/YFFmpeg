#pragma once

#include "ffmpeg.h"
#include <string>

class YParameters
{

public:

    YParameters();
    ~YParameters();

    void            setAvailable(bool available);
    void            setCodec(AVCodecID codec_id);
    void            setCodec(std::string _codec_short_name);
    void            setBitrate(int64_t bitrate);
    void            setDuration(int64_t duration);
    void            setStreamIndex(int64_t stream_index);

    bool            available()     const;
    AVCodecID       codecId()       const;
    std::string     codecName()     const;
    int64_t         bitrate()       const;
    int64_t         duration()      const;
    int64_t         streamIndex()   const;

protected:

    bool				_available;
    AVCodecID           _codec_id;
    std::string			_codec_name;
    int64_t             _bitrate;
    int64_t             _duration;
    int64_t             _stream_index;

};

