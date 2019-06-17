#pragma once

#include "base/YAbstractCodec.h"
#include "base/YDataProcessor.h"
#include "../context/YSource.h"

class YSource; //TODO

class YAudioDecoder : public YAbstractCodec, public YDataProcessor<YPacket,YFrame>
{

public:

    YAudioDecoder() = delete;
    YAudioDecoder(YSource *source);
    virtual ~YAudioDecoder() override;

    bool init() override;

private:

    bool copyCodecPar(AVFormatContext* input_format_context, AVMediaType media_tipe, int64_t stream_index, AVCodecContext** codec_context); //TODO arg list
    virtual YCode processInputData(YPacket& input_data) override;

private:

    //General parametres
    YSource*       _source;

};
