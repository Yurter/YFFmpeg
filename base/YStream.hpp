#pragma once

#include "YParameters.hpp"
#include "YPacket.hpp"
#include <vector>
#include <list>

class YStream : public YData<AVStream*>
{

public:

    YStream(const YStream& other)  = delete;
    YStream(const YStream&& other) = delete;
    YStream(YParameters* param = new YParameters());
    YStream(AVStream* stream, YParameters* param);
    virtual ~YStream() override;

    virtual YCode       init() override;
    virtual std::string toString() const override final;
    virtual YCode       stampPacket(YPacket& packet) final;

    void                setContext(YObject* context); //TOOD заменить на YContext* ?
    void                setUid(int64_t uid);
    void                setUsed(bool used);

    YObject*            context()   const;
    int64_t             uid()       const;
    int64_t             index()     const;
    int64_t             duration()  const;
    bool                used()      const;

    AVCodecParameters*  codecParameters();

    void                increaseDuration(int64_t value);

    virtual bool        operator>(const YStream& other) const; //TODO func betterThen(YStream* other) ? нет, максимум дублировать, т.к. испольуется в std::max_element

public:

    YParameters*        parameters;

protected:

    virtual void        parseParametres();

protected:

    YObject*            _context;

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

typedef std::list<YStream*>             StreamList;
typedef std::vector<YStream*>           StreamVector;
typedef std::pair<YStream*,YStream*>    StreamPair;
