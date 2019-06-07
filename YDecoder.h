#pragma once

#include "base/YAbstractCoder.h"
#include "base/YDataProcessor.h"
#include "YSource.h"

class YDecoder : public YAbstractCoder, public YDataProcessor<YPacket,YFrame>
{

public:

    YDecoder() = delete;
    YDecoder(YSource *source);
    virtual ~YDecoder() override;

    bool init() override;

private:

    bool initVideoCodec() override;
    bool initAudioCodec() override;
    bool copyCodecPar(AVFormatContext* input_format_context, AVMediaType media_tipe, int64_t stream_index, AVCodecContext** codec_context); //TODO arg list
    virtual YCode processInputData(YPacket& input_data) override;

private:

    //General parametres
    YSource*       _source;

};
