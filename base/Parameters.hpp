#pragma once
#include "MediaData.hpp"

namespace fpp {

    class Parameters;
    using SharedParameters = std::shared_ptr<Parameters>;
    using IOParams = struct { SharedParameters in; SharedParameters out; };

    enum ParamsType { //TODO rename 16.01
        Input,
        Output,
    };

    class Parameters : public MediaData {

    public:

        Parameters(ParamsType type);
        Parameters(const Parameters&) = default;
        virtual ~Parameters() override = default;

        Parameters& operator=(const Parameters&) = default;

        void                setCodec(AVCodecID codec_id);
        void                setCodec(std::string _codec_short_name);
        void                setCodec(AVCodec* codec);
        void                setBitrate(int64_t bitrate);
        void                setDuration(int64_t duration);
        void                setStreamIndex(uid_t stream_index);
        void                setTimeBase(AVRational time_base);
        void                setContextUid(uid_t context_uid);

        AVCodecID           codecId()       const;
        std::string         codecName()     const;
        AVCodec*            codec()         const;
        int64_t             bitrate()       const;
        int64_t             duration()      const;
        uid_t               streamIndex()   const;
        uid_t               streamUid()     const;
        AVRational          timeBase()      const;
        int64_t             contextUid()    const;

        void                increaseDuration(const int64_t value);

        virtual std::string toString() const override;

        virtual void        completeFrom(const SharedParameters other_params);
        virtual void        parseStream(const AVStream* avstream);
        virtual void        initStream(AVStream* avstream) const;
        virtual bool        betterThen(const SharedParameters& other);

    private:

        void                setStreamUid(uid_t value);
        void                setCodecType(CodecType value);

    protected:

        AVCodec*            _codec;
        AVCodecID           _codec_id;
        std::string			_codec_name;
        ParamsType          _io_type;
        int64_t             _bitrate;
        int64_t             _duration;
        uid_t               _stream_index;
        uid_t               _stream_uid;
        AVRational          _time_base;
        int64_t             _context_uid;

    };

} // namespace fpp
