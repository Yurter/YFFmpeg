#pragma once
#include "Data.hpp"

namespace fpp {

    class Packet : public Data<AVPacket> {

    public:

        Packet();
        Packet(const Packet& other);
        Packet(const AVPacket& avpacket, MediaType type);
        virtual ~Packet() override;

        Packet& operator=(const Packet& other);

        void                setPts(int64_t pts);
        void                setDts(int64_t dts);
        void                setDuration(int64_t duration);
        void                setTimeBase(AVRational time_base);
        void                setPos(int64_t pos);
        void                setStreamIndex(int64_t stream_index);

        int64_t             pts()           const;
        int64_t             dts()           const;
        int64_t             duration()      const;
        AVRational          timeBase()      const;
        int64_t             pos()           const;
        int64_t             streamIndex()   const;
        bool                keyFrame()      const;

        virtual uint64_t    size()      const override;
        virtual std::string toString()  const override;

    private:

        void                copy(const Packet& other);
        void                copy(const AVPacket& other, MediaType type);

    private:

        AVRational          _time_base;

    };

} // namespace fpp
