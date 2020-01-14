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

    class Stream : public Data<AVStream*> { //TODO сделать шаред и вик поинтеры 14.01

    public:

        Stream(const Stream& other)  = delete;
        Stream(const Stream&& other) = delete;
        Stream(Parameters* param = new Parameters()); //TODO memeory leak 14.01
        Stream(AVStream* stream, Parameters* param);

        //Stream(Parameters* param); TODO оставить отлько эти два конструктора 14.01
        Stream(const AVStream* avstream);

        virtual ~Stream() override;

        virtual Code        init() override;
        virtual std::string toString() const override final;

        void                determineStampType(const Packet& packet);
        Code                stampPacket(Packet& packet/*, AVRational packet_time_base*/);
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

        virtual bool        operator>(const Stream& other) const; //TODO func betterThen(Stream* other) ? нет, максимум дублировать, т.к. испольуется в std::max_element

    public:

        Parameters*         params;

    protected:

//        virtual void        parseStream();
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
