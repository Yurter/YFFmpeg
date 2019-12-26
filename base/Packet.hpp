#pragma once
#include "Data.hpp"

namespace fpp {

    class Packet : public Data<AVPacket> { //TODO вынести общее от пакета и фрейса в отдельный класс (птс, time_base, стрим_айди и тд)

    public:

        Packet();
        Packet(const Packet& other);
        Packet(const Packet&& other);
        Packet(const AVPacket& avpacket);
        virtual ~Packet() override;

        Packet& operator=(const Packet& other);
        Packet& operator=(const Packet&& other);

        virtual Code        init() override;

        void                setPts(int64_t pts);
        void                setDts(int64_t dts);
        void                setDuration(int64_t duration);
        void                setTimeBase(AVRational time_base);
        void                setPos(int64_t pos);
        void                setStreamIndex(int64_t stream_index);
        void                setStreamUid(int64_t source_stream_uid);

        int64_t             pts()           const;
        int64_t             dts()           const;
        int64_t             duration()      const;
        AVRational          timeBase()      const;
        int64_t             pos()           const;
        int64_t             streamIndex()   const;
        int64_t             streamUid()     const;
        bool                keyFrame()      const;

        virtual uint64_t    size()      const override;
        virtual std::string toString()  const override;

    private:

        void                copyOther(const Packet& other);

    private:

        int64_t             _stream_uid;
        AVRational          _time_base;

    };

} // namespace fpp
