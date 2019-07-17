#pragma once

#include "YMediaData.hpp"

template <typename Type>
class YData : public YMediaData
{

public:

    YData() :
        YData(Type())
    {
        EMPTY_CONSTRUCTOR
    }

    YData(Type data, YMediaType type = YMediaType::MEDIA_TYPE_UNKNOWN) :
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
