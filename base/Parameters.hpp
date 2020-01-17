#pragma once
#include "MediaData.hpp"

namespace fpp {

//    using Parameters = std::shared_ptr<_Parameters>; //TODO

    class Parameters;
    using ParametersPointer = std::shared_ptr<Parameters>;
    using IOParams = struct { ParametersPointer in; ParametersPointer out; };

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
        void                setStreamIndex(UID stream_index);
        void                setTimeBase(AVRational time_base);
        void                setContextUid(int64_t context_uid);

        AVCodecID           codecId()       const;
        std::string         codecName()     const;
        AVCodec*            codec()         const;
        int64_t             bitrate()       const;
        int64_t             duration()      const;
        UID                 streamIndex()   const;
        UID                 streamUid()     const;
        AVRational          timeBase()      const;
        int64_t             contextUid()    const;

        void                increaseDuration(const int64_t value);

        virtual std::string toString() const override;

        virtual Code        completeFrom(const ParametersPointer other_params);
        virtual void        parseStream(const AVStream* avstream);
        virtual void        initStream(AVStream* avstream) const;
        virtual bool        betterThen(const ParametersPointer& other);

    private:

        void                setStreamUid(UID value);
        void                setCodecType(CodecType value);

    protected:

        AVCodec*            _codec;
        AVCodecID           _codec_id;
        std::string			_codec_name;
        ParamsType          _io_type;
        int64_t             _bitrate;
        int64_t             _duration;
        UID                 _stream_index;
        UID                 _stream_uid;
        AVRational          _time_base;
        int64_t             _context_uid;

    };

} // namespace fpp
