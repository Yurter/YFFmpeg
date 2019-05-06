#pragma once

#include "YAbstractCoder.h"
#include "YMediaSource.h"

class YMediaDecoder : public YAbstractCoder
{
public:

    YMediaDecoder() = delete;
    YMediaDecoder(YMediaSource *source);
    virtual ~YMediaDecoder();

    bool init();

    std::list<AVFrame> decodePacket(AVPacket &&packet);

private:

    bool initVideoCodec();
    bool initAudioCodec();
    bool copyCodecPar(AVFormatContext *input_format_context, AVMediaType media_tipe, AVCodec *codec, AVCodecContext *codec_context);

private:

    //General parametres
    YMediaSource *_source;

};
