#pragma once
#include <base/Parameters.hpp>

namespace fpp {

    class AudioParameters : public Parameters {

    public:

        AudioParameters();
        virtual ~AudioParameters() override = default;

        void                setSampleRate(int64_t sample_rate);
        void                setSampleFormat(ffmpeg::AVSampleFormat sample_format);
        void                setChannelLayout(uint64_t channels_layout);
        void                setChannels(int64_t channels);
        void                setFrameSize(int64_t value);

        int64_t             sampleRate()    const;
        ffmpeg::AVSampleFormat  sampleFormat()  const;
        uint64_t            channelLayout() const;
        int64_t             channels()      const;
        int64_t             frameSize()     const;

        std::string         toString() const override;

        virtual void        completeFrom(const SharedParameters other_params)   override;
        virtual void        parseStream(const ffmpeg::AVStream* avstream, ParamsType type)       override;
        virtual bool        betterThen(const SharedParameters& other)           override;

    private:

        int64_t                 _sample_rate;
        ffmpeg::AVSampleFormat  _sample_format;
        uint64_t                _channel_layout;
        int64_t                 _channels;
        int64_t                 _frame_size;

    };

    using SharedAudioParameters = std::shared_ptr<AudioParameters>;

} // namespace fpp
