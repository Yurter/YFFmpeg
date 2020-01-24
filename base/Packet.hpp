#pragma once
#include <base/Data.hpp>

namespace ffmpeg { extern "C" {
    #include <libavcodec/avcodec.h>
} } // namespace ffmpeg

namespace fpp {

    class Packet : public Data<ffmpeg::AVPacket> {

    public:

        Packet();
        Packet(const Packet& other);
        Packet(const ffmpeg::AVPacket& avpacket, ffmpeg::AVRational time_base, MediaType type);
        virtual ~Packet() override;

        Packet& operator=(const Packet& other);

        void                setPts(int64_t pts);
        void                setDts(int64_t dts);
        void                setDuration(int64_t duration);
        void                setTimeBase(ffmpeg::AVRational time_base);
        void                setPos(int64_t pos);
        void                setStreamIndex(int64_t stream_index);

        int64_t             pts()           const;
        int64_t             dts()           const;
        int64_t             duration()      const;
        ffmpeg::AVRational  timeBase()      const;
        int64_t             pos()           const;
        int64_t             streamIndex()   const;
        bool                keyFrame()      const;

        virtual size_t      size()      const override;
        virtual std::string toString()  const override;

    private:

        void                copy(const Packet& other);
        void                copy(const ffmpeg::AVPacket& other, ffmpeg::AVRational time_base, MediaType type);

    private:

        ffmpeg::AVRational  _time_base;

    };

} // namespace fpp
