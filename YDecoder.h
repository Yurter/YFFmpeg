#pragma once

#include "base/YAbstractCoder.h"
#include "base/YDataProcessor.h"
#include "YSource.h"

class YMediaDecoder : public YAbstractCoder, public YDataProcessor<YPacket,YFrame>
{

public:

    YMediaDecoder() = delete;
    YMediaDecoder(YMediaSource *source);
    virtual ~YMediaDecoder() override;

    bool init() override;

private:

    bool initVideoCodec() override;
    bool initAudioCodec() override;
    bool copyCodecPar(AVFormatContext *input_format_context, AVMediaType media_tipe, int64_t stream_index, AVCodecContext **codec_context); //TODO arg list
    YCode run() override;

private:

    //General parametres
    YMediaSource*       _source;

};
