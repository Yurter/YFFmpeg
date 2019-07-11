#pragma once

#include "YParameters.h"
#include "YPacket.h"

class YStream : public YData<AVStream*>
{

public:

    YStream(const YStream& other) = delete;
    YStream(const YStream&& other) = delete;
    YStream(YParameters* param = new YParameters());
    YStream(AVStream* stream, YParameters* param);
    virtual ~YStream() override;

    virtual YCode       init() override;
    virtual std::string toString() const override final;
    virtual YCode       stampPacket(YPacket& packet) final;

    void                setUid(int64_t uid);
//    void                setTimeBase(AVRational time_base);

    int64_t             uid()       const;
    int64_t             index()     const;
    int64_t             duration()  const;
//    AVRational          timeBase()  const;

    AVCodecParameters*  codecParameters();
    void                increaseDuration(int64_t value);

public:

    YParameters*        parameters;

protected:

    virtual void        parseParametres();

protected:

    // General
    int64_t             _uid;
    int64_t             _duration;
    int64_t             _prev_dts;
    int64_t             _prev_pts;
    int64_t             _packet_index;

    int64_t             _packet_dts_delta;
    int64_t             _packet_pts_delta;
    int64_t             _packet_duration;

};

typedef std::pair<YStream*,YStream*> streams_pair;
