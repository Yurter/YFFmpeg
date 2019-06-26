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

//void utils::codecpar_from_parameters(AVCodecParameters* codecpar, YParameters& parameters) //TODO
//{
//    AVCodecContext sample = qwe;
//    codecpar->codec_type = parameters.codecType();
//    codecpar->codec_id   = parameters.codecId();
//    codecpar->codec_tag  = parameters.codecTag();

//    codecpar->bit_rate              = parameters.bitrate();
//    codecpar->bits_per_coded_sample = parameters.bitsPerCodedSample();
//    codecpar->bits_per_raw_sample   = parameters.bitsPerRawSample();
//    codecpar->profile               = parameters.profile();
//    codecpar->level                 = parameters.level();

//    switch (codecpar->codec_type) {
//    case AVMEDIA_TYPE_VIDEO:
//        auto video_parametres = YVideoParameters(parameters);
//        codecpar->format              = video_parametres.pixelFormat();
//        codecpar->width               = video_parametres.width();
//        codecpar->height              = video_parametres.height();
//        codecpar->field_order         = video_parametres.fieldOrder();
//        codecpar->color_range         = video_parametres.colorRange();
//        codecpar->color_primaries     = video_parametres.colorPrimaries();
//        codecpar->color_trc           = video_parametres.colorTrc();
//        codecpar->color_space         = video_parametres.colorspace();
//        codecpar->chroma_location     = video_parametres.chromaSampleLocation();
//        codecpar->sample_aspect_ratio = video_parametres.sampleAspectRatio();
//        codecpar->video_delay         = video_parametres->hasBframes();
//        break;
//    case AVMEDIA_TYPE_AUDIO:
//        auto audio_parametres = YAudioParameters(parameters);
//        codecpar->format           = audio_parametres.sampleFormat();
//        codecpar->channel_layout   = audio_parametres.channelLayout();
//        codecpar->channels         = audio_parametres.channels();
//        codecpar->sample_rate      = audio_parametres.sampleRate();
//        codecpar->block_align      = audio_parametres.blockAlign();
//        codecpar->frame_size       = audio_parametres.frameSize();
//        codecpar->initial_padding  = audio_parametres.initialPadding();
//        codecpar->trailing_padding = audio_parametres.trailingPadding();
//        codecpar->seek_preroll     = audio_parametres.seekPreroll();
//        break;
//    }
//}

YCode utils::init_codecpar(AVCodecParameters* codecpar, AVCodec* codec)
{
    auto codec_context = avcodec_alloc_context3(codec);
    return_if(not_inited_ptr(codec_context), YCode::ERR);
    return_if(avcodec_parameters_from_context(codecpar, codec_context) < 0, YCode::ERR);
    avcodec_free_context(&codec_context);
    return YCode::OK;
}

//void utils::codecpar_from_parameters(AVCodecParameters* codecpar, YParameters& parameters) //TODO
//{
//    auto codec = parameters.codec();
//    codecpar->codec_type = codec->codec_type;
//    codecpar->codec_id   = codec->codec_id;
//    codecpar->codec_tag  = codec->codec_tag;

//    codecpar->bit_rate              = codec->bit_rate;
//    codecpar->bits_per_coded_sample = codec->bits_per_coded_sample;
//    codecpar->bits_per_raw_sample   = codec->bits_per_raw_sample;
//    codecpar->profile               = codec->profile;
//    codecpar->level                 = codec->level;

//    switch (codecpar->codec_type) {
//    case AVMEDIA_TYPE_VIDEO:
//        codecpar->format              = codec->pix_fmt;
//        codecpar->width               = codec->width;
//        codecpar->height              = codec->height;
//        codecpar->field_order         = codec->field_order;
//        codecpar->color_range         = codec->color_range;
//        codecpar->color_primaries     = codec->color_primaries;
//        codecpar->color_trc           = codec->color_trc;
//        codecpar->color_space         = codec->colorspace;
//        codecpar->chroma_location     = codec->chroma_sample_location;
//        codecpar->sample_aspect_ratio = codec->sample_aspect_ratio;
//        codecpar->video_delay         = codec->has_b_frames;
//        break;
//    case AVMEDIA_TYPE_AUDIO:
//        codecpar->format           = codec->sample_fmt;
//        codecpar->channel_layout   = codec->channel_layout;
//        codecpar->channels         = codec->channels;
//        codecpar->sample_rate      = codec->sample_rate;
//        codecpar->block_align      = codec->block_align;
//        codecpar->frame_size       = codec->frame_size;
//        codecpar->initial_padding  = codec->initial_padding;
//        codecpar->trailing_padding = codec->trailing_padding;
//        codecpar->seek_preroll     = codec->seek_preroll;
//        break;
//    }
//}
