#pragma once

#include "ffmpeg.h"
#include "YObject.h"
#include <string>

template <typename Type>
class YData : public YObject
{

public:

    YData() :
        _type(YMediaType::MEDIA_TYPE_UNKNOWN) { setName("YData"); }
    YData(Type data, YMediaType type) :
        _type(type),
        _data(data){ setName("YData"); }
    virtual ~YData() {}

    Type&           raw() { return _data; }

    YMediaType      type() const { return _type; }
    void            setType(YMediaType type) { _type = type; }

    bool            isVideo() const { return _type == YMediaType::MEDIA_TYPE_VIDEO; }
    bool            isAudio() const { return _type == YMediaType::MEDIA_TYPE_AUDIO; }

    virtual bool        init() {}
    virtual bool        inited()    const = 0;
    virtual std::string toString()  const = 0;

protected:

    // General
    YMediaType      _type;

    // FFmpeg
    Type            _data;
};

