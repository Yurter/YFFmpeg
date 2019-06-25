#pragma once

#include "YParameters.h"
#include "YPacket.h"

class YAbstractMedia; //TODO YAbstractMedia::_streams to pointers ?

class YStream : public YData<AVStream*>
{

public:

    YStream(YParameters parameters = YParameters());
    YStream(YAbstractMedia* media_context, AVStream* stream, YMediaType type, YParameters parameters);
    virtual ~YStream() override = default;

    virtual YCode       init() override;
    virtual bool        inited() const override final;
    virtual std::string toString() const override final;
    virtual YCode       stampPacket(YPacket& packet) final;

    void                setUid(int64_t uid);

    int64_t             uid() const;
    int64_t             index()     const;
    int64_t             duration()  const;
    AVRational          timeBase()  const;
    AVCodecParameters*  codecParameters();
    YAbstractMedia*     mediaContext();
    void                increaseDuration(int64_t value);

protected:

    // Media
    YParameters         _parameters;
    YAbstractMedia*     _media_context;

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
