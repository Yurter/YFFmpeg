#pragma once
#include "MediaData.hpp"

namespace fpp {

    using StreamId_t = int64_t;

    class Parameters : public MediaData {

    public:

        Parameters(/*MediaType type = MediaType::Unknown*/); //TODO убрать параметр ? 15.01
        virtual ~Parameters() override = default;

        void                setCodec(AVCodecID codec_id, CodecType codec_type);
        void                setCodec(std::string _codec_short_name, CodecType codec_type);
        void                setCodec(AVCodec* codec);
        void                setBitrate(int64_t bitrate);
        void                setDuration(int64_t duration);
        void                setStreamIndex(StreamId_t stream_index);
        void                setTimeBase(AVRational time_base);
        void                setContextUid(int64_t context_uid);

        AVCodecID           codecId()       const;
        std::string         codecName()     const;
        AVCodec*            codec()         const;
        CodecType           codecType()     const;
        int64_t             bitrate()       const;
        int64_t             duration()      const;
        StreamId_t          streamIndex()   const;
        StreamId_t          streamUid()     const;
        AVRational          timeBase()      const;
        int64_t             contextUid()    const;

        void                increaseDuration(const int64_t value);

        virtual std::string toString() const override;

        virtual Code        completeFrom(const Parameters* other_params);
        virtual void        parseStream(const AVStream* avstream);

    private:

        void                setStreamUid(int64_t value);
        void                setCodecType(CodecType value);

    protected:

        AVCodec*            _codec;
        AVCodecID           _codec_id;
        std::string			_codec_name;
        CodecType           _codec_type;
        int64_t             _bitrate;
        int64_t             _duration;
        StreamId_t          _stream_index;
        StreamId_t          _stream_uid;
        AVRational          _time_base;
        int64_t             _context_uid;

    };

//    using IOParams = struct { const Parameters * const in; const Parameters * const out; }; константа убрана из-за необходимости устанваливать фрейм сайз из энкодера
//    using IOParams = struct { Parameters * const in; Parameters * const out; };
    using ParametersPointer = std::shared_ptr<Parameters>;
    using IOParams = struct { ParametersPointer in; ParametersPointer out; };


} // namespace fpp
