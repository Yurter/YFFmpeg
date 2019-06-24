#pragma once

#include "YData.h"
//#include "YStream.h"
#include "utils.h"

class YStream;

class YPacket : public YData<AVPacket>
{

public:

    YPacket();
    virtual ~YPacket() override;

    bool            init() override;
    bool            inited() const override;

    void            setPts(int64_t pts);
    void            setDts(int64_t dts);
    void            setDuration(int64_t duration);
    void            setPos(int64_t pos);
    void            setStreamIndex(int64_t stream_index);
    void            setStream(YStream* stream);

    int64_t         pts()           const;
    int64_t         dts()           const;
    int64_t         duration()      const;
    int64_t         pos()           const;
    int64_t         streamIndex()   const;
    int64_t         sourceIndex()   const;
    YStream*        stream();

    bool            empty()         const;
    std::string     toString()      const override;

private:

    // Media
    YStream*        _stream;

};
