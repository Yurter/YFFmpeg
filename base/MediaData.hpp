#pragma once
#include "Object.hpp"

namespace fpp {

    class MediaData : public Object {

    public:

        MediaData(MediaType type = MediaType::MEDIA_TYPE_UNKNOWN);
        virtual ~MediaData() override = default;

        MediaType       type() const { return _type; }
        void            setType(MediaType type) { _type = type; }

        bool            isVideo() const { return _type == MediaType::MEDIA_TYPE_VIDEO; }
        bool            isAudio() const { return _type == MediaType::MEDIA_TYPE_AUDIO; }
        bool            typeIs(MediaType media_type) { return _type == media_type; }

    private:

        MediaType          _type;

    };

} // namespace fpp
