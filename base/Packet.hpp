#pragma once

#include "Data.hpp"

namespace fpp {

    class Packet : public Data<AVPacket> {

    public:

        Packet();
        virtual ~Packet() override;

        virtual Code        init() override;

        void                setPts(int64_t pts);
        void                setDts(int64_t dts);
        void                setDuration(int64_t duration);
        void                setPos(int64_t pos);
        void                setStreamIndex(int64_t stream_index);
        void                setStreamUid(int64_t source_stream_uid);

        int64_t             pts()           const;
        int64_t             dts()           const;
        int64_t             duration()      const;
        int64_t             pos()           const;
        int64_t             streamIndex()   const;
        int64_t             streamUid()     const;

        bool                empty()         const;
        virtual std::string toString()      const override;

    private:

        int64_t             _stream_uid;

    };

} // namespace fpp
