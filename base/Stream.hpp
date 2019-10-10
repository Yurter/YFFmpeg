#pragma once
#include "Parameters.hpp"
#include "Packet.hpp"
#include <vector>
#include <list>

namespace fpp {

    class Stream : public Data<AVStream*> {

    public:

        Stream(const Stream& other)  = delete;
        Stream(const Stream&& other) = delete;
        Stream(Parameters* param = new Parameters());
        Stream(AVStream* stream, Parameters* param);
        virtual ~Stream() override;

        virtual Code        init() override;
        virtual std::string toString() const override final;
        virtual Code        stampPacket(Packet& packet) final;

        void                setContext(Object* context); //TOOD заменить на Context* ?
        void                setUid(int64_t uid);
        void                setUsed(bool used);

        Object*             context()   const;
        int64_t             uid()       const;
        int64_t             index()     const;
        int64_t             duration()  const;
        bool                used()      const;

        AVCodecParameters*  codecParameters();

        void                increaseDuration(int64_t value);

        virtual bool        operator>(const Stream& other) const; //TODO func betterThen(Stream* other) ? нет, максимум дублировать, т.к. испольуется в std::max_element

    public:

        Parameters*         parameters;

    protected:

        virtual void        parseParametres();

    protected:

        Object*             _context;

        int64_t             _uid;
        int64_t             _duration;
        bool                _used;

        int64_t             _prev_dts;
        int64_t             _prev_pts;
        int64_t             _packet_index;

        int64_t             _packet_dts_delta;
        int64_t             _packet_pts_delta;
        int64_t             _packet_duration;

    };

//    typedef std::list<Stream*>          StreamList;
//    typedef std::vector<Stream*>        StreamVector;
//    typedef std::pair<Stream*,Stream*>  StreamPair;
    using StreamList = std::list<Stream*>;
    using StreamVector = std::vector<Stream*>;
    using StreamPair = std::pair<Stream*,Stream*>;

} // namespace fpp
