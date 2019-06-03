#pragma once

#include "base/YAbstractCoder.h"
#include "YMediaSource.h"

class YMediaDecoder : public YAbstractCoder
{

public:

    YMediaDecoder() = delete;
    YMediaDecoder(YMediaSource *source);
    virtual ~YMediaDecoder();

    bool init();

    YCode decodePacket(YPacket& packet, AVFrame** decoded_frame);

private:

    bool initVideoCodec();
    bool initAudioCodec();
    bool copyCodecPar(AVFormatContext *input_format_context, AVMediaType media_tipe, int64_t stream_index, AVCodecContext **codec_context); //TODO arg list
    void run();

private:

    //General parametres
    YMediaSource*       _source;

};
