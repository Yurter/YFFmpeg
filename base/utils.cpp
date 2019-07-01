#include "utils.h"
#include <thread>

std::string utils::media_type_to_string(YMediaType media_type)
{
    switch (media_type) {
    case YMediaType::MEDIA_TYPE_UNKNOWN:
        return "Unknown";
    case YMediaType::MEDIA_TYPE_VIDEO:
        return "Video";
    case YMediaType::MEDIA_TYPE_AUDIO:
        return "Audio";
    }
    return "Invalid";
}

std::string utils::pts_to_string(int64_t pts)
{
    return pts == AV_NOPTS_VALUE ? "NOPTS" : std::to_string(pts);
}

void utils::sleep_for(int64_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

bool utils::exit_code(YCode code)
{
    if (code == YCode::ERR)             { return true; }
    if (code == YCode::NOT_INITED)      { return true; }
    if (code == YCode::END_OF_FILE)     { return true; }
    if (code == YCode::INVALID_INPUT)   { return true; }
    return false;
}

std::string utils::code_to_string(YCode code)
{
    if (code == YCode::OK)              { return "OK";              }
    if (code == YCode::ERR)             { return "Error";           }
    if (code == YCode::AGAIN)           { return "Again";           }
    if (code == YCode::NOT_INITED)      { return "Not inited";      }
    if (code == YCode::END_OF_FILE)     { return "EOF";             }
    if (code == YCode::INVALID_INPUT)   { return "Invalid input";   }
    return "Unknown error code: " + std::to_string(code);
}

std::string utils::rational_to_string(AVRational rational)
{
    std::string str = std::to_string(rational.num)
                        + "/"
                        + std::to_string(rational.den);
    return str;
}

bool utils::compatibleWithSampleFormat(AVCodecContext *codec_context, AVSampleFormat sample_format)
{
    auto smp_fmt = codec_context->codec->sample_fmts;
    while (smp_fmt[0] != AV_SAMPLE_FMT_NONE) {
        if (smp_fmt[0] == sample_format) { return true; }
        smp_fmt++;
    }
    return false;
}

AVMediaType utils::ymedia_type_to_avmedia_type(YMediaType media_type)
{
    switch (media_type) {
    case YMediaType::MEDIA_TYPE_UNKNOWN:
        return AVMEDIA_TYPE_UNKNOWN;
    case YMediaType::MEDIA_TYPE_VIDEO:
        return AVMEDIA_TYPE_VIDEO;
    case YMediaType::MEDIA_TYPE_AUDIO:
        return AVMEDIA_TYPE_AUDIO;
    }
}

int64_t utils::gen_context_uid()
{
    return object_uid_handle++;
}

int64_t utils::gen_stream_uid(int64_t context_uid, int64_t stream_index)
{
    return ((context_uid + 1) * 100) + stream_index;
}

std::string utils::guess_format_short_name(std::string media_resurs_locator)
{
    if (media_resurs_locator.find("rtsp://") != std::string::npos) {
        return std::string("rtsp");
    }
    if (media_resurs_locator.find("rtmp://") != std::string::npos) {
        return std::string("flv");
    }
    if (media_resurs_locator.find("aevalsrc") != std::string::npos) {
        return std::string("lavfi");
    }
    return std::string();
}

YCode utils::init_codecpar(AVCodecParameters* codecpar, AVCodec* codec)
{
    auto codec_context = avcodec_alloc_context3(codec);
    return_if(not_inited_ptr(codec_context), YCode::ERR);
    return_if(avcodec_parameters_from_context(codecpar, codec_context) < 0, YCode::ERR);
    avcodec_free_context(&codec_context);
    return YCode::OK;
}

void utils::parameters_to_context(YParameters* parametres, AVCodecContext* codec)
{
    codec->codec_id = parametres->codecId();
    codec->bit_rate = parametres->bitrate();

    switch (parametres->type()) {
    case YMediaType::MEDIA_TYPE_VIDEO: {
        auto video_parameters = dynamic_cast<YVideoParameters*>(parametres);
        codec->pix_fmt                = video_parameters->pixelFormat();
        codec->width                  = int(video_parameters->width());
        codec->height                 = int(video_parameters->height());
//        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
        break;
    }
    case YMediaType::MEDIA_TYPE_AUDIO: {
        auto audio_parameters = dynamic_cast<YAudioParameters*>(parametres);
        codec->sample_fmt       = audio_parameters->sampleFormat();
        codec->channel_layout   = audio_parameters->channelLayout();
        codec->channels         = int(audio_parameters->channels());
        codec->sample_rate      = int(audio_parameters->sampleRate());
        break;
    }
    case YMediaType::MEDIA_TYPE_UNKNOWN:
        break;
    }
}

void utils::parameters_from_context(YParameters* parametres, AVCodecContext* codec)
{
    parametres->setCodec(codec->codec_id);
    parametres->setBitrate(codec->bit_rate);

    switch (parametres->type()) {
    case YMediaType::MEDIA_TYPE_VIDEO: {
        auto video_parameters = dynamic_cast<YVideoParameters*>(parametres);
        video_parameters->setPixelFormat(codec->pix_fmt);
        video_parameters->setWidth(codec->width);
        video_parameters->setHeight(codec->height);
//        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
        break;
    }
    case YMediaType::MEDIA_TYPE_AUDIO: {
        auto audio_parameters = dynamic_cast<YAudioParameters*>(parametres);
        audio_parameters->setSampleFormat(codec->sample_fmt);
        audio_parameters->setChannelLayout(codec->channel_layout);
        audio_parameters->setChannels(codec->channels);
        audio_parameters->setSampleRate(codec->sample_rate);
        break;
    }
    case YMediaType::MEDIA_TYPE_UNKNOWN:
        break;
    }
}

void utils::parameters_to_avcodecpar(YParameters* parametres, AVCodecParameters* codecpar) //TODO
{
    codecpar->codec_id = parametres->codecId();
    codecpar->bit_rate = parametres->bitrate();

    switch (parametres->type()) {
    case YMediaType::MEDIA_TYPE_VIDEO: {
        auto video_parameters = dynamic_cast<YVideoParameters*>(parametres);
        codecpar->width                  = int(video_parameters->width());
        codecpar->height                 = int(video_parameters->height());
//        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
        break;
    }
    case YMediaType::MEDIA_TYPE_AUDIO: {
        auto audio_parameters = dynamic_cast<YAudioParameters*>(parametres);
        codecpar->channel_layout   = audio_parameters->channelLayout();
        codecpar->channels         = int(audio_parameters->channels());
        codecpar->sample_rate      = int(audio_parameters->sampleRate());
        break;
    }
    case YMediaType::MEDIA_TYPE_UNKNOWN:
        break;
    }
}

void utils::parameters_from_avcodecpar(YParameters* parametres, AVCodecParameters* codecpar) //TODO
{
    parametres->setCodec(codecpar->codec_id);
    parametres->setBitrate(codecpar->bit_rate);

    switch (parametres->type()) {
    case YMediaType::MEDIA_TYPE_VIDEO: {
        auto video_parameters = dynamic_cast<YVideoParameters*>(parametres);
//        video_parameters->setPixelFormat(codecpar->pix_fmt);
        video_parameters->setWidth(codecpar->width);
        video_parameters->setHeight(codecpar->height);
//        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
        break;
    }
    case YMediaType::MEDIA_TYPE_AUDIO: {
        auto audio_parameters = dynamic_cast<YAudioParameters*>(parametres);
//        audio_parameters->setSampleFormat(codecpar->sample_fmt);
        audio_parameters->setChannelLayout(codecpar->channel_layout);
        audio_parameters->setChannels(codecpar->channels);
        audio_parameters->setSampleRate(codecpar->sample_rate);
        break;
    }
    case YMediaType::MEDIA_TYPE_UNKNOWN:
        break;
    }
}
