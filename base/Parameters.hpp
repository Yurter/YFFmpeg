#pragma once
#include <base/MediaData.hpp>

#define DEFAULT_TIME_BASE ffmpeg::AVRational { 1, 1000 }

namespace ffmpeg {
    struct AVStream;
} // namespace ffmpeg

namespace ffmpeg { extern "C" {
    #include <libavcodec/avcodec.h>
} } // namespace ffmpeg

namespace fpp {

    /* ? */
    enum class CodecType { //TODO конфликт имён
        Unknown,
        Decoder,
        Encoder,
    };

    class Parameters;
    using SharedParameters = std::shared_ptr<Parameters>;
    using IOParams = struct { SharedParameters in; SharedParameters out; };

    enum ParamsType { //TODO rename 16.01
        Input,
        Output,
    };

    class Parameters : public MediaData {

    public:

        Parameters();
        Parameters(const Parameters&) = default;
        virtual ~Parameters() override = default;

        Parameters& operator=(const Parameters&) = default;

        void                setDecoder(ffmpeg::AVCodecID codec_id);
        void                setEncoder(ffmpeg::AVCodecID codec_id);

        void                setBitrate(int64_t bitrate);
        void                setDuration(int64_t duration);
        void                setStreamIndex(uid_t stream_index);
        void                setTimeBase(ffmpeg::AVRational time_base);
        void                setContextUid(uid_t context_uid);

        ffmpeg::AVCodecID   codecId()       const;
        std::string         codecName()     const;
        ffmpeg::AVCodec*    codec()         const;
        int64_t             bitrate()       const;
        int64_t             duration()      const;
        uid_t               streamIndex()   const;
        uid_t               streamUid()     const;
        ffmpeg::AVRational  timeBase()      const;
        int64_t             contextUid()    const;

        void                increaseDuration(const int64_t value);

        virtual std::string toString() const override;

        virtual void        completeFrom(const SharedParameters other_params);
        virtual void        parseStream(const ffmpeg::AVStream* avstream, ParamsType type);
        virtual void        initStream(ffmpeg::AVStream* avstream) const;
        virtual bool        betterThen(const SharedParameters& other);

    private:

        void                setCodec(ffmpeg::AVCodec* codec);
        void                setStreamUid(uid_t value);

    protected:

        ffmpeg::AVCodec*    _codec;
        int64_t             _bitrate;
        int64_t             _duration;
        uid_t               _stream_index;
        uid_t               _stream_uid;
        ffmpeg::AVRational  _time_base;
        int64_t             _context_uid;

    };

} // namespace fpp
