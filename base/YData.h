#pragma once

#include "ffmpeg.h"
#include "utils.h"
#include "YLogger.h"
#include "YMediaData.h"

template <typename Type>
class YData : public YMediaData
{

public:

    YData() :
        YData(Type(), YMediaType::MEDIA_TYPE_UNKNOWN)
    { }

    YData(Type data, YMediaType type) :
        YMediaData(type),
        _data(data)
    {
        setName("YData");
    }

    virtual ~YData() override = default;

    void                setRaw(Type data) { _data = data; }
    Type&               raw() { return _data; }

protected:

    Type                _data;
};
