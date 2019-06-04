#include "YMediaDecoder.h"
#include <iostream>

YMediaDecoder::YMediaDecoder(YMediaSource *source) :
    _source(source)
{
    //
}

YMediaDecoder::~YMediaDecoder()
{
    //
}

bool YMediaDecoder::init()
{
    if (_source->video_parameters.available() && !_source->video_parameters.ignore()) {
        if (!initVideoCodec()) {
            std::cerr << "[YMediaDecoder] Failed to init video codec" << std::endl;
            return false;
        }
    }
    if (_source->audio_parameters.available() && !_source->audio_parameters.ignore()) {
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

void YMediaDecoder::pushPacket(YPacket &packet)
{
    _packet_queue.push(packet);
}

bool YMediaDecoder::popFrame(YFrame &frame)
{
    return _frame_queue.pop(frame);
}

bool YMediaDecoder::initVideoCodec()
{
    bool copied = copyCodecPar(_source->mediaFormatContext(), AVMEDIA_TYPE_VIDEO, _source->video_parameters.streamIndex(), &_video_codec_context);
    if (!copied) {
        std::cerr << "[YMediaDecoder] Video codec copy error" << std::endl;
    }
    _video_stream_index = _source->video_parameters.streamIndex();
    return copied;
}

bool YMediaDecoder::initAudioCodec()
{
    bool copied = copyCodecPar(_source->mediaFormatContext(), AVMEDIA_TYPE_AUDIO, _source->audio_parameters.streamIndex(), &_audio_codec_context);
    if (!copied) {
        std::cerr << "[YMediaDecoder] Audio codec copy error" << std::endl;
    }
    _audio_stream_index = _source->audio_parameters.streamIndex();
    /* Crutch */ //TODO
    _audio_codec_context->channel_layout = static_cast<uint64_t>(av_get_default_channel_layout(_audio_codec_context->channels));
    return copied;
}

bool YMediaDecoder::copyCodecPar(AVFormatContext *input_format_context, AVMediaType media_tipe, int64_t stream_index, AVCodecContext **codec_context)
{
    AVCodec *codec;
    if (av_find_best_stream(input_format_context, media_tipe, -1, -1, &codec, 0) < 0) {
        std::cerr << "[YMediaDecoder] Cannot find an av stream in the input file" << std::endl;
        return false;
    }
    *codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        std::cerr << "[YMediaDecoder] Failed to alloc context" << std::endl;
        return false;
    }
    if (avcodec_parameters_to_context(*codec_context, input_format_context->streams[stream_index]->codecpar) < 0) {
        std::cerr << "[YMediaDecoder] avcodec_parameters_to_context failed" << std::endl;
        return false;
    }
    if (avcodec_open2(*codec_context, codec, nullptr) < 0) {
        std::cerr << "[YMediaDecoder] Cannot open av decoder" << std::endl;
        return false;
    }
    return true;
}

YCode YMediaDecoder::run()
{
    YPacket packet;
    if (!_packet_queue.pop(packet)) {
        utils::sleep_for(SHORT_DELAY_MS);
        return YCode::AGAIN;
    }
    AVCodecContext *codec_context = nullptr;
    if (packet.isVideo()) { codec_context = _video_codec_context; }
    if (packet.isAudio()) { codec_context = _audio_codec_context; }
    if (codec_context == nullptr) {
        std::cerr << "[YMediaDecoder] codec_context is null" << std::endl;
        return YCode::ERR;
    }
    if (!packet.empty()) {
        if (avcodec_send_packet(codec_context, &packet.raw()) != 0) {
            std::cerr << "[YMediaDecoder] Could not send packet" << std::endl;
            return YCode::ERR;
        }
    }
    YFrame decoded_frame;
    decoded_frame.alloc();
    int ret = avcodec_receive_frame(codec_context, decoded_frame.raw());
    switch (ret) {
    case 0:
        decoded_frame.setType(packet.type());
        _frame_queue.push(decoded_frame);
        return YCode::OK;
    case AVERROR(EAGAIN):
        return YCode::AGAIN;
    case AVERROR_EOF:
        return YCode::END_OF_FILE;
    case AVERROR(EINVAL):
        return YCode::INVALID_INPUT;
    default:
        return YCode::ERR;
    }
}
