#pragma once
#include "base/Stream.hpp"
#include "AudioParameters.hpp"

namespace fpp {

    class AudioStream : public Stream {

    public:

        AudioStream(AudioParameters* param = new AudioParameters());
        AudioStream(AVStream* stream, AudioParameters* param);
        virtual ~AudioStream() override = default;

        Code                init() override;

        bool                operator>(const Stream& other) const override;

    };

//    typedef std::list<AudioStream*> AudioStreamList;
    using AudioStreamList = std::list<AudioStream*>;

} // namespace fpp
