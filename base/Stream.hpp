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

        void                setContext(Object* context); //TODO заменить на Context* ? //TODO убарть
        void                setUid(int64_t uid);
        void                setUsed(bool used);

        Object*             context()       const;
        int64_t             index()         const;
        bool                used()          const;
        int64_t             packetIndex()   const;

        AVCodecParameters*  codecParameters();

        int64_t packet_duration() const { return _packet_duration; } //TODO

        virtual bool        operator>(const Stream& other) const; //TODO func betterThen(Stream* other) ? нет, максимум дублировать, т.к. испольуется в std::max_element

    public:

        Parameters*         params;

    protected:

        virtual void        parseParameters();

    protected:

        bool                _used;
        Object*             _context; //TODO заменить на shared_ptr ? в пайплайне возникает проблема двойного удаления при формировании смартпои

        int64_t             _prev_dts;
        int64_t             _prev_pts;
        int64_t             _packet_index;

        int64_t             _packet_dts_delta;
        int64_t             _packet_pts_delta;
        int64_t             _packet_duration;

    };

    using StreamList = std::list<Stream*>;
    using StreamVector = std::vector<Stream*>;
    using StreamPair = std::pair<Stream*,Stream*>;

} // namespace fpp
