#pragma once

#include "YData.h"

class YPacket : public YData<AVPacket>
{

public:

    YPacket();
    virtual ~YPacket() override;

    YCode           init() override;
    bool            inited() const override;

    void            setPts(int64_t pts);
    void            setDts(int64_t dts);
    void            setDuration(int64_t duration);
    void            setPos(int64_t pos);
    void            setStreamIndex(int64_t stream_index);
    void            setStreamUid(int64_t source_stream_uid);

    int64_t         pts()           const;
    int64_t         dts()           const;
    int64_t         duration()      const;
    int64_t         pos()           const;
    int64_t         streamIndex()   const;
    int64_t         streamUid()     const;

    bool            empty()         const;
    std::string     toString()      const override;

private:

    int64_t         _stream_uid;

};
