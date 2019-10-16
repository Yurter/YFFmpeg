#pragma once
#include "base/Parameters.hpp"

namespace fpp {

    class AudioParameters : public Parameters {

    public:

        AudioParameters();
        virtual ~AudioParameters() override = default;

        void                setSampleRate(int64_t sample_rate);
        void                setSampleFormat(AVSampleFormat sample_format);
        void                setChannelLayout(uint64_t channels_layout); // TODO скрыть в прайват? устанваливать дефолтное значение при установке числа каналов?
        void                setChannels(int64_t channels);

        int64_t             sampleRate()    const;
        AVSampleFormat      sampleFormat()  const;
        uint64_t            channelLayout() const;
        int64_t             channels()      const;

        std::string         toString() const override;

    //    void                softCopy(Parameters* other_parametrs)  override;
        AudioParameters&   operator=(const AudioParameters& rhs);

    private:

        int64_t             _sample_rate;
        AVSampleFormat      _sample_format;
        uint64_t            _channel_layout;
        int64_t             _channels;

    };

} // namespace fpp