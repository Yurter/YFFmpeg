#include "YMediaEncoder.h"
#include <iostream>

YMediaEncoder::YMediaEncoder(YMediaDestination *destination) :
    _destination(destination)
{
    //
}

bool YMediaEncoder::init()
{
    if (_destination->videoAvailable()) {
        if (!initVideoCodec()) {
            std::cerr << "[YMediaDecoder] Failed to init video codec" << std::endl;
            return false;
        }
    }
    if (_destination->audioAvailable()) {
        if (!initAudioCodec()) {
            std::cerr << "[YMediaDecoder] Failed to init audio codec" << std::endl;
            return false;
        }
    }
    std::cout << "[YMediaEncoder] Inited" << std::endl;
    return true;
}

bool YMediaEncoder::encodeFrames(AVPacket *encoded_packet, std::list<AVFrame*> &decoded_frames)
{
    AVCodecContext *codec_context = nullptr;
    if (encoded_packet->stream_index == _video_stream_index) {
        codec_context = _video_codec_context;
    }
    if (encoded_packet->stream_index == _audio_stream_index) {
        codec_context = _audio_codec_context;
    }
    int ret;
    for (auto&& decoded_frame : decoded_frames) {
        ret = av_frame_make_writable(decoded_frame);
        if ((ret = avcodec_send_frame(codec_context, decoded_frame)) != 0) {
            std::cerr << "[YMediaEncoder] Could not send frame " << ret << std::endl;
            return false;
        }
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
//    AVCodec *decoder = avcodec_find_encoder_by_name(_destination->videoCodecName().c_str());
    AVCodec *decoder = avcodec_find_encoder(_destination->videoCodecId());
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


    if (avcodec_open2(_video_codec_context, decoder, nullptr) < 0) {
        std::cerr << "[YMediaEncoder] Could not open video encoder" << std::endl;
        return false;
    }

    _destination->addStream(_video_codec_context);

    _video_stream_index = _destination->videoStreamIndex();

    return true;
}

bool YMediaEncoder::initAudioCodec()
{
    AVCodec *encoder = avcodec_find_encoder(_destination->audioCodecId());
    if (encoder == nullptr) {
        std::cerr << "[YMediaEncoder] Could not find audio encoder " << _destination->audioCodecName() << std::endl;
        return false;
    } else {
        //
    }
    _audio_codec_context = avcodec_alloc_context3(encoder);
    //
    // инициализвация параметров кодека
    //
    _audio_codec_context->codec_tag = 0;

    _audio_codec_context->sample_rate = static_cast<int>(_destination->sampleRate());
    _audio_codec_context->bit_rate = _destination->audioBitrate();
    _audio_codec_context->sample_fmt = _destination->sampleFormat();
    _audio_codec_context->channel_layout = _destination->audioChanelsLayout();
    _audio_codec_context->channels = static_cast<int>(_destination->audioChanels());

//    _audio_codec_context->time_base = { 1, _audio_codec_context->sample_rate };


//    av_opt_set(_audio_codec_context, "profile", "aac_low", 0);
    //av_opt_set(_audio_codec_context, "b", "384k", 0);

//    av_opt_set(_audio_codec_context, "frame_size ", "4096", 0);


    // Audio: pcm_mulaw, 16000 Hz, mono, s16, 128 kb/s
    // Audio: aac (LC) ([10][0][0][0] / 0x000A), 16000 Hz, mono, fltp, 69 kb/s

    if (avcodec_open2(_audio_codec_context, encoder, nullptr) < 0) {
        std::cerr << "[YMediaEncoder] Could not open audio encoder" << std::endl;
        return false;
    }

//    _audio_codec_context->frame_size = 4096;

    _destination->addStream(_audio_codec_context);

    _audio_stream_index = _destination->audioStreamIndex();

    return true;
}
