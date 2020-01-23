#pragma once
#include <core/Object.hpp>

namespace fpp {

    /* Медиа тип потока/пакета/фрейма */
    enum class MediaType { //TODO убрать инициализацию полей и методы дискад и игнорм (этот функционал переходит под ответственность потоков) 14.01
        Unknown = 0x0001,
        Video   = 0x0002,
        Audio   = 0x0004,
        EndOF   = 0x0008,
    };

    class MediaData : public Object {

    public:

        MediaData(MediaType type = MediaType::Unknown)
            : _type(type) {
            setName("MediaData");
        }
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
