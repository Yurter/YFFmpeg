#pragma once

#include "YObject.hpp"

class YMediaData : public YObject
{

public:

    YMediaData(YMediaType type = YMediaType::MEDIA_TYPE_UNKNOWN);
    virtual ~YMediaData() override = default;

    YMediaType          type() const { return _type; }
    void                setType(YMediaType type) { _type = type; }

    bool                isVideo() const { return _type == YMediaType::MEDIA_TYPE_VIDEO; }
    bool                isAudio() const { return _type == YMediaType::MEDIA_TYPE_AUDIO; }
    bool                typeIs(YMediaType media_type) { return _type == media_type; }

private:

    YMediaType          _type;

};

