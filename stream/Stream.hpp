#pragma once
#include "base/Parameters.hpp"
#include "core/time/Chronometer.hpp"
#include "base/Packet.hpp"
#include <vector>
#include <list>

namespace fpp {

//    enum class StreamType { //TODO crutch ? 16.01
//        Output,
//        Input,
//    };

    class Stream;
    using SharedStream = std::shared_ptr<Stream>;
    using StreamVector = std::vector<SharedStream>;

//    static SharedStream createInputStream();
//    static SharedStream createOutputStream();

    class Stream : public Data<const AVStream*> {


    public:

        Stream(const AVStream* avstream, ParametersPointer parameters);
        Stream(const AVStream* avstream, ParamsType type);   // Создание реального потока
        Stream(ParametersPointer params);   // Создание виртуального потока
        Stream(const Stream& other)  = delete;
        virtual ~Stream() override = default;

        virtual Code        init() override;
        virtual std::string toString() const override final;

        void                determineStampType(const Packet& packet);
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

        AVCodecParameters*  codecParameters();

    public:

        ParametersPointer   params;

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
