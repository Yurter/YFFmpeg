#pragma once
#include "base/Parameters.hpp"

namespace fpp {

    class AudioParameters : public Parameters {

    public:

        AudioParameters();
        virtual ~AudioParameters() override = default;

        void                setSampleRate(int64_t sample_rate);
        void                setSampleFormat(AVSampleFormat sample_format);
        void                setChannelLayout(uint64_t channels_layout);
        void                setChannels(int64_t channels);
        void                setFrameSize(int64_t value);

        int64_t             sampleRate()    const;
        AVSampleFormat      sampleFormat()  const;
        uint64_t            channelLayout() const;
        int64_t             channels()      const;
        int64_t             frameSize()     const;

        std::string         toString() const override;

        Code                completeFrom(const ParametersPointer other_params)  override;
        virtual void        parseStream(const AVStream* avstream)               override;
        virtual bool        betterThen(const ParametersPointer& other)          override;

    private:

        int64_t             _sample_rate;
        AVSampleFormat      _sample_format;
        uint64_t            _channel_layout;
        int64_t             _channels;
        int64_t             _frame_size;

    };

} // namespace fpp
