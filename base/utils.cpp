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
    return (context_uid * 100) + stream_index;
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
    codec->codec_type = parametres->codec_type;
    codec->codec_id   = pparametresar->codec_id;
    codec->codec_tag  = parametres->codec_tag;

    codec->bit_rate              = parametres->bit_rate;
    codec->bits_per_coded_sample = parametres->bits_per_coded_sample;
    codec->bits_per_raw_sample   = parametres->bits_per_raw_sample;
    codec->profile               = parametres->profile;
    codec->level                 = parametres->level;

    switch (parametres->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        codec->pix_fmt                = parametres->format;
        codec->width                  = parametres->width;
        codec->height                 = parametres->height;
        codec->field_order            = parametres->field_order;
        codec->color_range            = parametres->color_range;
        codec->color_primaries        = parametres->color_primaries;
        codec->color_trc              = parametres->color_trc;
        codec->colorspace             = parametres->color_space;
        codec->chroma_sample_location = parametres->chroma_location;
        codec->sample_aspect_ratio    = parametres->sample_aspect_ratio;
        codec->has_b_frames           = parametres->video_delay;
        break;
    case AVMEDIA_TYPE_AUDIO:
        codec->sample_fmt       = parametres->format;
        codec->channel_layout   = parametres->channel_layout;
        codec->channels         = parametres->channels;
        codec->sample_rate      = parametres->sample_rate;
        codec->block_align      = parametres->block_align;
        codec->frame_size       = parametres->frame_size;
        codec->delay            =
        codec->initial_padding  = parametres->initial_padding;
        codec->trailing_padding = parametres->trailing_padding;
        codec->seek_preroll     = parametres->seek_preroll;
        break;
    case AVMEDIA_TYPE_SUBTITLE:
        codec->width  = parametres->width;
        codec->height = parametres->height;
        break;
    }
}

void utils::parameters_from_context(YParameters *parametres, AVCodecContext *codec)
{
    //
}

void utils::parameters_to_avcodecpar(YParameters *parametres, AVCodecParameters *codecpar)
{
    //
}

void utils::parameters_from_avcodecpar(YParameters *parametres, AVCodecParameters *codecpar)
{
    //
}
