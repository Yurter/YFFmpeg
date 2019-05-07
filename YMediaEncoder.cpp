#include "YMediaEncoder.h"
#include <iostream>

YMediaEncoder::YMediaEncoder(YMediaDestination *destination) : _destination(destination)
{
    //
}

bool YMediaEncoder::init()
{
    std::cout << "[YMediaEncoder] Initializing" << std::endl;
    initVideoCodec();
    initAudioCodec();
    return true;
}

AVPacket YMediaEncoder::encodeFrames(std::list<AVFrame> &&frames)
{
    AVPacket packet;
    return packet;
}

YMediaEncoder::~YMediaEncoder()
{
    //
}

bool YMediaEncoder::initVideoCodec()
{
    AVCodec *decoder = avcodec_find_decoder_by_name(_destination->videoCodecName().c_str());
    if (decoder == nullptr) {
        std::cerr << "[YMediaDestination] Could not find video decoder " << _destination->videoCodecName() << std::endl;
        return false;
    }
    AVCodecContext* avCodecContext = avcodec_alloc_context3(decoder);
    //
    // инициализвация параметров кодека
    //
    avCodecContext->codec_tag = 0;

    int frames_per_second = static_cast<int>(_destination->frameRate());
    avCodecContext->width = static_cast<int>(_destination->width());
    avCodecContext->height = static_cast<int>(_destination->height());
    avCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    avCodecContext->sample_aspect_ratio = {1,1};//_destination->aspectRatio();
    avCodecContext->gop_size = frames_per_second * 2;
    avCodecContext->framerate = { frames_per_second, 1 };
    avCodecContext->time_base = { 1, frames_per_second };
    avCodecContext->max_b_frames = 2;
    //avCodecContext->bit_rate

    //Video: h264 (Main), yuvj420p(pc, bt709, progressive), 1920x1080 [SAR 1:1 DAR 16:9], 23 fps, 25 tbr, 90k tbn, 46 tbc
    //Video: flv1 (flv) ([2][0][0][0] / 0x0002), yuv420p, 1920x1080 [SAR 1:1 DAR 16:9], q=2-31, 200 kb/s, 25 fps, 1k tbn, 25 tbc

    _destination->addStream(avCodecContext);

    return true;
}

bool YMediaEncoder::initAudioCodec()
{
    AVCodec *decoder = avcodec_find_decoder_by_name(_destination->audioCodecName().c_str());
    if (decoder == nullptr) {
        std::cerr << "[YMediaDestination] Could not find audio decoder " << _destination->videoCodecName() << std::endl;
        return false;
    } else {
        //
    }
    AVCodecContext* avCodecContext = avcodec_alloc_context3(decoder);
    //
    // инициализвация параметров кодека
    //
    avCodecContext->codec_tag = 0;

    avCodecContext->sample_rate = 44'100;
    avCodecContext->sample_fmt = AV_SAMPLE_FMT_FLTP;
    avCodecContext->channel_layout = AV_CH_LAYOUT_STEREO;
    avCodecContext->bit_rate = 192 * 1'024;


    // Audio: pcm_mulaw, 16000 Hz, mono, s16, 128 kb/s
    // Audio: aac (LC) ([10][0][0][0] / 0x000A), 16000 Hz, mono, fltp, 69 kb/s

    _destination->addStream(avCodecContext);

    return true;
}
