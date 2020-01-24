#pragma once
#include <base/Parameters.hpp>
#include <core/time/Chronometer.hpp>
#include <base/Packet.hpp>
#include <vector>

#define FROM_START  0
#define TO_END      LONG_MAX

namespace fpp {

    /* ? */
    enum class StampType {
        /* Штампы сорса */
        Copy,
        Realtime,
        Offset,
        /* Штампы синка */
        Rescale,
    };

    class Stream;
    using SharedStream = std::shared_ptr<Stream>;
    using StreamVector = std::vector<SharedStream>;

    class Stream : public Data<const ffmpeg::AVStream*> {


    public:

        Stream(const ffmpeg::AVStream* avstream, SharedParameters parameters); //TODO сделать приватным 23.01
        Stream(const ffmpeg::AVStream* avstream, ParamsType type);  // Создание реального потока
        Stream(SharedParameters params);                    // Создание виртуального потока    //TODO не используется 24.01
        Stream(const Stream& other)  = delete;
        virtual ~Stream() override = default;

        virtual void        init() override;
        virtual std::string toString() const override final;

        Code                stampPacket(Packet& packet);
        bool                timeIsOver() const;

        void                setUsed(bool value);
        void                setStampType(StampType value);
        void                setStartTimePoint(int64_t value);
        void                setEndTimePoint(int64_t value);

        int64_t             index()             const;
        bool                used()              const;
        StampType           stampType()         const;
        int64_t             startTimePoint()    const;
        int64_t             endTimePoint()      const;
        int64_t             packetIndex()       const;

        ffmpeg::AVCodecParameters*  codecParameters();

    public:

        SharedParameters    params;

    protected:

        bool                _used;
        StampType           _stamp_type;
        Chronometer         _chronometer;

        int64_t             _prev_dts;
        int64_t             _prev_pts;
        int64_t             _packet_index;

        int64_t             _packet_dts_delta;
        int64_t             _packet_pts_delta;
        int64_t             _packet_duration;

        int64_t             _pts_offset = 0;
        int64_t             _dts_offset = 0;

        int64_t             _start_time_point;
        int64_t             _end_time_point;

    };

} // namespace fpp
