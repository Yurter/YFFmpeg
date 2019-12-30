#pragma once
#include "Parameters.hpp"
#include "core/time/Chronometer.hpp"
#include "Packet.hpp"
#include <vector>
#include <list>

namespace fpp {

    //TODO
    class Processor;
    using ProcessorPointer = std::shared_ptr<Processor>;

    enum class StampType {
        /* Штампы сорса */
        Copy,
        Realtime,
        Offset,
        /* Штампы синка */
        Rescale,
    };
//    enum class StampType {
//        ConstantFramerate,
//        VariableFramerate,
//        Copy,
//        Append,
//        Rescale,
//        Realtime,
//        Offset,
//    };

    class Stream : public Data<AVStream*> {

    public:

        Stream(const Stream& other)  = delete;
        Stream(const Stream&& other) = delete;
        Stream(Parameters* param = new Parameters());
        Stream(AVStream* stream, Parameters* param);
        virtual ~Stream() override;

        virtual Code        init() override;
        virtual std::string toString() const override final;

        void                determineStampType(const Packet& packet);
        Code                stampPacket(Packet& packet);
        bool                timeIsOver() const;

        void                setUsed(bool value);
        void                setStampType(StampType value);
        void                setStartTimePoint(int64_t value);
        void                setEndTimePoint(int64_t value);

        int64_t             index()         const;
        bool                used()          const;
        StampType           stampType()     const;
        int64_t             startTimePoint() const;
        int64_t             endTimePoint()   const;
        int64_t             packetIndex()   const;

        AVCodecParameters*  codecParameters();

        virtual bool        operator>(const Stream& other) const; //TODO func betterThen(Stream* other) ? нет, максимум дублировать, т.к. испольуется в std::max_element

    public:

        Parameters*         params;

    protected:

        virtual void        parseParameters();

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

    using StreamList = std::list<Stream*>;
    using StreamVector = std::vector<Stream*>;
    using StreamPair = std::pair<Stream*,Stream*>;

} // namespace fpp
