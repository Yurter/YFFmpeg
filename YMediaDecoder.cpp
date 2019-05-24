#include "YMediaDecoder.h"
#include <iostream>

YMediaDecoder::YMediaDecoder(YMediaSource *source) :
    _source(source)
{
    //
}

bool YMediaDecoder::init()
{
    if (_source->video_parameters.available()) {
        if (!initVideoCodec()) {
            std::cerr << "[YMediaDecoder] Failed to init video codec" << std::endl;
            return false;
        }
    }
    if (_source->audio_parameters.available()) {
        if (!initAudioCodec()) {
            std::cerr << "[YMediaDecoder] Failed to init audio codec" << std::endl;
            return false;
        }
    }
    if (_source->mediaFormatContext()->nb_streams == 0) {
        std::cerr << "[YMediaDecoder] Source wasn't inited" << std::endl;
        return false;
    }
    std::cout << "[YMediaDecoder] Inited" << std::endl;
    return true;
}

bool YMediaDecoder::decodePacket(AVPacket *packet, std::list<AVFrame*> &decoded_frames)
{
    AVCodecContext *codec_context = nullptr;
    if (packet->stream_index == _video_stream_index) {
        codec_context = _video_codec_context;
    }
    if (packet->stream_index == _audio_stream_index) {
        codec_context = _audio_codec_context;
    }
    if (avcodec_send_packet(codec_context, packet) != 0) {
        std::cerr << "[YMediaDecoder] Could not send packet" << std::endl;
        return false;
    }
    //decoded_frames.clear();
    AVFrame *decoded_frame = av_frame_alloc();
//    int ret = av_frame_make_writable(decoded_frame);
//    decoded_frame->nb_samples = 1024;
    while (avcodec_receive_frame(codec_context, decoded_frame) >= 0) {
        decoded_frames.push_back(decoded_frame);
        decoded_frame = av_frame_alloc();
    }
    return true;
}

bool YMediaDecoder::initVideoCodec()
{
    std::cout << "[YMediaDecoder] Copying video codec" << std::endl;
    bool copied = copyCodecPar(_source->mediaFormatContext(), AVMEDIA_TYPE_VIDEO, _source->video_parameters.streamIndex(), &_video_codec_context);
    if (!copied) {
        std::cerr << "[YMediaDecoder] Video codec copy error" << std::endl;
    }
    _video_stream_index = _source->video_parameters.streamIndex();
    return copied;
}

bool YMediaDecoder::initAudioCodec()
{
    std::cout << "[YMediaDecoder] Copying audio codec" << std::endl;
    bool copied = copyCodecPar(_source->mediaFormatContext(), AVMEDIA_TYPE_AUDIO, _source->audio_parameters.streamIndex(), &_audio_codec_context);
    if (!copied) {
        std::cerr << "[YMediaDecoder] Audio codec copy error" << std::endl;
    }
    _audio_stream_index = _source->audio_parameters.streamIndex();
    return copied;
}

bool YMediaDecoder::copyCodecPar(AVFormatContext *input_format_context, AVMediaType media_tipe, int64_t stream_index, AVCodecContext **codec_context)
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
    avcodec_parameters_to_context(*codec_context, input_format_context->streams[stream_index]->codecpar);
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
