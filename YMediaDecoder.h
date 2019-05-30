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

    bool decodePacket(AVPacket *packet, std::list<AVFrame*> &decoded_frames);

private:

    bool initVideoCodec();
    bool initAudioCodec();
    bool copyCodecPar(AVFormatContext *input_format_context, AVMediaType media_tipe, int64_t stream_index, AVCodecContext **codec_context);

private:

    //General parametres
    YMediaSource*   _source;

};
