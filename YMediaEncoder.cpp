#include "YMediaEncoder.h"
#include <iostream>

YMediaEncoder::YMediaEncoder(YMediaDestination *destination) :
    _destination(destination)
{
    //
}

bool YMediaEncoder::init()
{
    std::cout << "[YMediaEncoder] Initializing" << std::endl;
    bool encoder_inited = initVideoCodec() && initAudioCodec();
    std::cerr << "[DEBUG] " << _video_codec_context << " | " << _audio_codec_context << std::endl;
    return encoder_inited;
}

bool YMediaEncoder::encodeFrames(AVPacket *encoded_packet, std::list<AVFrame*> &decoded_frames)
{
    AVCodecContext *codec_context = nullptr;
    if (encoded_packet->stream_index == AVMEDIA_TYPE_VIDEO) {
        codec_context = _video_codec_context;
    }
    if (encoded_packet->stream_index == AVMEDIA_TYPE_AUDIO) {
        codec_context = _audio_codec_context;
    }
    int ret;
    for (auto&& decoded_frame : decoded_frames) {
//        if (encoded_packet->stream_index == AVMEDIA_TYPE_AUDIO) {
//            decoded_frame->sample_rate = codec_context->sample_rate;
//            decoded_frame->format = codec_context->sample_fmt;
//            decoded_frame->nb_samples = codec_context->frame_size;
//            decoded_frame->channel_layout = 2;//_audio_codec_context->channel_layout;
//            decoded_frame->channels = 2;
//        }
//        decoded_frame.pts = 0;
//        decoded_frame.pkt_dts = 0;
        ret = -1;
//        if (!decoded_frame) {
//            return false;
//        }
//        auto empty_frame = av_frame_alloc();
//        ret = av_frame_make_writable(decoded_frame);
//        av_frame_copy(empty_frame, decoded_frame);
//        if ((ret = avcodec_send_frame(codec_context, empty_frame)) != 0) {
//            std::cerr << "[YMediaEncoder] Could not send frame " << ret << std::endl;
//            return false;
//        }
        try {
            if ((ret = avcodec_send_frame(codec_context, decoded_frame)) != 0) {
                std::cerr << "[YMediaEncoder] Could not send frame " << ret << std::endl;
                return false;
            }
        } catch (...) {}
    }
    if ((ret = avcodec_receive_packet(codec_context, encoded_packet)) != 0) {
        char* text_error = new char[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, text_error, AV_ERROR_MAX_STRING_SIZE);
        std::cerr << "[YMediaEncoder] Could not receive packet " << ret << " " << AVERROR(EAGAIN) << " " << std::string(text_error) << std::endl;
        return false;
    }
    return true;
}

YMediaEncoder::~YMediaEncoder()
{
    //
}

bool YMediaEncoder::initVideoCodec()
{
    AVCodec *decoder = avcodec_find_encoder_by_name(_destination->videoCodecName().c_str());
    if (decoder == nullptr) {
        std::cerr << "[YMediaEncoder] Could not find video encoder " << _destination->videoCodecName() << std::endl;
        return false;
    }
    _video_codec_context = avcodec_alloc_context3(decoder);
    //
    // инициализвация параметров кодека
    //
    _video_codec_context->codec_tag = 0;

    int frames_per_second = static_cast<int>(_destination->frameRate());
    _video_codec_context->width = static_cast<int>(_destination->width());
    _video_codec_context->height = static_cast<int>(_destination->height());
    _video_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
    _video_codec_context->sample_aspect_ratio = {1,1};//_destination->aspectRatio();
    _video_codec_context->gop_size = 10;//frames_per_second * 2;
    _video_codec_context->framerate = { frames_per_second, 1 };
    _video_codec_context->time_base = { 1, frames_per_second };
    _video_codec_context->max_b_frames = 1;//2;

    _video_codec_context->bit_rate = 400000;// = static_cast<int64_t>(_destination->width() * _destination->height() * 3);
    _video_codec_context->global_quality = 1;
    _video_codec_context->compression_level = FF_COMPRESSION_DEFAULT;

    _video_codec_context->ticks_per_frame = 2;

    _video_codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;


    if (decoder->id == AV_CODEC_ID_H264) {
        av_opt_set(_video_codec_context->priv_data, "preset", "ultrafast", 0);
    }



//    av_opt_set(avCodecContext, "size", "1920:1080", 0);
//    av_opt_set(avCodecContext, "s", "2000:900", 0);
//    av_opt_set(avCodecContext, "profile", "Baseline", 0);
//    av_opt_set(avCodecContext, "profile", "high", 0);
//    av_opt_set(avCodecContext, "bf", "2", 0);
//    av_opt_set(avCodecContext, "coder", "1", 0);
//    av_opt_set_int(avCodecContext, "coder", 1, 0);
//    av_opt_set(avCodecContext, "crf", "18", 0);


    if (avcodec_open2(_video_codec_context, decoder, nullptr) < 0) {
        std::cerr << "[YMediaEncoder] Could not open video encoder" << std::endl;
        return false;
    }

    _destination->addStream(_video_codec_context);

    return true;
}

bool YMediaEncoder::initAudioCodec()
{
    AVCodec *decoder = avcodec_find_encoder_by_name(_destination->audioCodecName().c_str());
    if (decoder == nullptr) {
        std::cerr << "[YMediaEncoder] Could not find audio encoder " << _destination->audioCodecName() << std::endl;
        return false;
    } else {
        //
    }
    _audio_codec_context = avcodec_alloc_context3(decoder);
    //
    // инициализвация параметров кодека
    //
    _audio_codec_context->codec_tag = 0;

    _audio_codec_context->sample_rate = 16000;//44'100;
    _audio_codec_context->sample_fmt = AV_SAMPLE_FMT_FLTP;//AV_SAMPLE_FMT_FLTP;//AV_SAMPLE_FMT_S16
    _audio_codec_context->channel_layout = AV_CH_LAYOUT_MONO;//AV_CH_LAYOUT_STEREO;
    _audio_codec_context->channels = 1;//2;
    _audio_codec_context->bit_rate = 192 * 1'024;

//    _audio_codec_context->time_base = { 1, _audio_codec_context->sample_rate };


    av_opt_set(_audio_codec_context, "profile", "aac_low", 0);
    //av_opt_set(_audio_codec_context, "b", "384k", 0);


    // Audio: pcm_mulaw, 16000 Hz, mono, s16, 128 kb/s
    // Audio: aac (LC) ([10][0][0][0] / 0x000A), 16000 Hz, mono, fltp, 69 kb/s

    if (avcodec_open2(_audio_codec_context, decoder, nullptr) < 0) {
        std::cerr << "[YMediaEncoder] Could not open audio encoder" << std::endl;
        return false;
    }

    _destination->addStream(_audio_codec_context);

    return true;
}
