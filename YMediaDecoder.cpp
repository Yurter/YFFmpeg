#include "YMediaDecoder.h"
#include <iostream>

YMediaDecoder::YMediaDecoder(YMediaSource *source) :
    _source(source)
{
    //
}

bool YMediaDecoder::init()
{
    std::cout << "[YMediaDecoder] Initializing" << std::endl;
    if (!initVideoCodec()) {
        std::cerr << "[YMediaDecoder] Failed to init video codec" << std::endl;
        return false;
    }
    if (!initAudioCodec()) {
        std::cerr << "[YMediaDecoder] Failed to init audio codec" << std::endl;
        return false;
    }
    return true;
}

bool YMediaDecoder::decodePacket(AVPacket *packet, std::list<AVFrame*> &decoded_frames)
{
    AVCodecContext *codec_context = nullptr;
    if (packet->stream_index == AVMEDIA_TYPE_VIDEO) {
        codec_context = _video_codec_context;
    }
    if (packet->stream_index == AVMEDIA_TYPE_AUDIO) {
        codec_context = _audio_codec_context;
    }
    if (avcodec_send_packet(codec_context, packet) != 0) {
        std::cerr << "[YMediaDecoder] Could not send packet" << std::endl;
        return false;
    }
    //decoded_frames.clear();
    AVFrame *decoded_frame = av_frame_alloc();
    while (avcodec_receive_frame(codec_context, decoded_frame) >= 0) {
        decoded_frames.push_back(decoded_frame);
        decoded_frame = av_frame_alloc();
    }
    return true;
}

bool YMediaDecoder::initVideoCodec()
{
    std::cout << "[YMediaDecoder] Copying video codec" << std::endl;
    bool copied = copyCodecPar(_source->mediaFormatContext(), AVMEDIA_TYPE_VIDEO, &_video_codec_context);
    if (!copied) {
        std::cerr << "[YMediaDecoder] Video codec copy error" << std::endl;
    }
    return copied;
}

bool YMediaDecoder::initAudioCodec()
{
    std::cout << "[YMediaDecoder] Copying audio codec" << std::endl;
    bool copied = copyCodecPar(_source->mediaFormatContext(), AVMEDIA_TYPE_AUDIO, &_audio_codec_context);
    if (!copied) {
        std::cerr << "[YMediaDecoder] Audio codec copy error" << std::endl;
    }
    //
    _audio_codec_context->channel_layout = _audio_codec_context->channels;
    //
    return copied;
}

bool YMediaDecoder::copyCodecPar(AVFormatContext *input_format_context, AVMediaType media_tipe, AVCodecContext **codec_context)
{
    AVCodec *codec;
    if (av_find_best_stream(input_format_context, media_tipe, -1, -1, &codec, 0) < 0) {
        std::cerr << "Cannot find an av stream in the input file" << std::endl;
        return false;
    }
    *codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        std::cerr << "Failed to alloc context" << std::endl;
        return false;
    }
    avcodec_parameters_to_context(*codec_context, input_format_context->streams[media_tipe]->codecpar);
    if (avcodec_open2(*codec_context, codec, nullptr) < 0) {
        std::cerr << "Cannot open av decoder" << std::endl;
        return false;
    }
    std::cout << "[DEBUG] codec: " << codec->name << std::endl;
    return true;
}

YMediaDecoder::~YMediaDecoder()
{
    //
}
