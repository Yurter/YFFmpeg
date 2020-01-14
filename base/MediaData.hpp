#pragma once
#include "core/Object.hpp"

namespace fpp {

    class MediaData : public Object {

    public:

        MediaData(MediaType type = MediaType::Unknown);
        virtual ~MediaData() override = default;

        MediaType           type() const { return _type; }
        void                setType(MediaType type) { _type = type; }

        bool                isVideo() const { return _type == MediaType::Video; }
        bool                isAudio() const { return _type == MediaType::Audio; }
        bool                isEOF()   const { return _type == MediaType::EndOF; }

        bool                typeIs(MediaType media_type) const { return _type == media_type; }

    private:

        MediaType           _type;

    };

} // namespace fpp
