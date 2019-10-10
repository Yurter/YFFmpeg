#include "utils.hpp"
#include <thread>

namespace fpp {

    std::string utils::media_type_to_string(MediaType media_type) {
        switch (media_type) {
        case MediaType::MEDIA_TYPE_UNKNOWN:
            return "Unknown";
        case MediaType::MEDIA_TYPE_VIDEO:
            return "Video";
        case MediaType::MEDIA_TYPE_AUDIO:
            return "Audio";
        }
        return "Invalid";
    }

    std::string utils::pts_to_string(int64_t pts) {
        return pts == AV_NOPTS_VALUE ? "NOPTS" : std::to_string(pts);
    }

    void utils::sleep_for(int64_t milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    bool utils::exit_code(Code code) {
        if (code == Code::ERR)             { return true; }
        if (code == Code::NOT_INITED)      { return true; }
        if (code == Code::END_OF_FILE)     { return true; }
        if (code == Code::INVALID_INPUT)   { return true; }
        if (code == Code::EXCEPTION)       { return true; }
        return false;
    }

    std::string utils::code_to_string(Code code) {
        if (code == Code::OK)              { return "OK";              }
        if (code == Code::ERR)             { return "Error";           }
        if (code == Code::AGAIN)           { return "Again";           }
        if (code == Code::NOT_INITED)      { return "Not inited";      }
        if (code == Code::END_OF_FILE)     { return "EOF";             }
        if (code == Code::INVALID_INPUT)   { return "Invalid input";   }
        return "Unknown error code: " + std::to_string(code);
    }

    std::string utils::rational_to_string(AVRational rational) {
        std::string str = std::to_string(rational.num)
                            + "/"
                            + std::to_string(rational.den);
        return str;
    }

    bool utils::compatibleWithSampleFormat(AVCodecContext *codec_context, AVSampleFormat sample_format) {
        auto smp_fmt = codec_context->codec->sample_fmts;
        while (smp_fmt[0] != AV_SAMPLE_FMT_NONE) {
            if (smp_fmt[0] == sample_format) { return true; }
            smp_fmt++;
        }
        return false;
    }

    AVMediaType utils::ymedia_type_to_avmedia_type(MediaType media_type) {
        switch (media_type) {
        case MediaType::MEDIA_TYPE_UNKNOWN:
            return AVMEDIA_TYPE_UNKNOWN;
        case MediaType::MEDIA_TYPE_VIDEO:
            return AVMEDIA_TYPE_VIDEO;
        case MediaType::MEDIA_TYPE_AUDIO:
            return AVMEDIA_TYPE_AUDIO;
        }
        throw Exception("");
    }

    int64_t utils::gen_context_uid() {
        return object_uid_handle++;
    }

    int64_t utils::gen_stream_uid(int64_t context_uid, int64_t stream_index) {
        return (context_uid + 1) * 100 + stream_index;
    }

    int64_t utils::get_context_uid(int64_t stream_uid) {
        return stream_uid / 100 - 1;
    }

    std::string utils::guess_format_short_name(std::string media_resurs_locator) {
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

    AVCodec* utils::find_decoder(std::string codec_short_name) {
        return avcodec_find_decoder_by_name(codec_short_name.c_str());
    }

    AVCodec* utils::find_decoder(AVCodecID codec_id) {
        return avcodec_find_decoder(codec_id);
    }

    AVCodec* utils::find_encoder(std::string codec_short_name) {
        return avcodec_find_encoder_by_name(codec_short_name.c_str());
    }

    AVCodec* utils::find_encoder(AVCodecID codec_id) {
        return avcodec_find_encoder(codec_id);
    }

    Code utils::init_codecpar(AVCodecParameters* codecpar, AVCodec* codec) {
        auto codec_context = avcodec_alloc_context3(codec);
        return_if(not_inited_ptr(codec_context), Code::ERR);
        return_if(avcodec_parameters_from_context(codecpar, codec_context) < 0, Code::ERR);
        avcodec_free_context(&codec_context);
        return Code::OK;
    }

    void utils::parameters_to_context(Parameters* parametres, AVCodecContext* codec) {
        codec->codec_id = parametres->codecId();
        codec->bit_rate = parametres->bitrate();

        switch (parametres->type()) {
        case MediaType::MEDIA_TYPE_VIDEO: {
            auto video_parameters = dynamic_cast<VideoParameters*>(parametres);
            codec->pix_fmt      = video_parameters->pixelFormat();
            codec->width        = int(video_parameters->width());
            codec->height       = int(video_parameters->height());
            codec->time_base    = DEFAULT_TIME_BASE;
    //        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
            break;
        }
        case MediaType::MEDIA_TYPE_AUDIO: {
            auto audio_parameters = dynamic_cast<AudioParameters*>(parametres);
            codec->sample_fmt       = audio_parameters->sampleFormat();
            codec->channel_layout   = audio_parameters->channelLayout();
            codec->channels         = int(audio_parameters->channels());
            codec->sample_rate      = int(audio_parameters->sampleRate());
            break;
        }
        case MediaType::MEDIA_TYPE_UNKNOWN:
            break;
        }
    }

//    void utils::parameters_from_context(Parameters* parametres, AVCodecContext* codec) {
//        parametres->setCodec(codec->codec_id);
//        parametres->setBitrate(codec->bit_rate);

//        switch (parametres->type()) {
//        case MediaType::MEDIA_TYPE_VIDEO: {
//            auto video_parameters = dynamic_cast<VideoParameters*>(parametres);
//            video_parameters->setPixelFormat(codec->pix_fmt);
//            video_parameters->setWidth(codec->width);
//            video_parameters->setHeight(codec->height);
//    //        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
//            break;
//        }
//        case MediaType::MEDIA_TYPE_AUDIO: {
//            auto audio_parameters = dynamic_cast<AudioParameters*>(parametres);
//            audio_parameters->setSampleFormat(codec->sample_fmt);
//            audio_parameters->setChannelLayout(codec->channel_layout);
//            audio_parameters->setChannels(codec->channels);
//            audio_parameters->setSampleRate(codec->sample_rate);
//            break;
//        }
//        case MediaType::MEDIA_TYPE_UNKNOWN:
//            break;
//        }
//    }

    //TODO
    void utils::parameters_to_avcodecpar(Parameters* parametres, AVCodecParameters* codecpar) {
        codecpar->codec_id = parametres->codecId();
        codecpar->bit_rate = parametres->bitrate();

        switch (parametres->type()) {
        case MediaType::MEDIA_TYPE_VIDEO: {
            auto video_parameters = dynamic_cast<VideoParameters*>(parametres);
            codecpar->width                  = int(video_parameters->width());
            codecpar->height                 = int(video_parameters->height());
    //        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
            break;
        }
        case MediaType::MEDIA_TYPE_AUDIO: {
            auto audio_parameters = dynamic_cast<AudioParameters*>(parametres);
            codecpar->channel_layout   = audio_parameters->channelLayout();
            codecpar->channels         = int(audio_parameters->channels());
            codecpar->sample_rate      = int(audio_parameters->sampleRate());
            break;
        }
        case MediaType::MEDIA_TYPE_UNKNOWN:
            break;
        }
    }

    //TODO
    void utils::parameters_from_avcodecpar(Parameters* parametres, AVCodecParameters* codecpar) {
        parametres->setCodec(codecpar->codec_id);
        parametres->setBitrate(codecpar->bit_rate);

        switch (parametres->type()) {
        case MediaType::MEDIA_TYPE_VIDEO: {
            auto video_parameters = dynamic_cast<VideoParameters*>(parametres);
    //        video_parameters->setPixelFormat(codecpar->pix_fmt);
            video_parameters->setWidth(codecpar->width);
            video_parameters->setHeight(codecpar->height);
    //        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
            break;
        }
        case MediaType::MEDIA_TYPE_AUDIO: {
            auto audio_parameters = dynamic_cast<AudioParameters*>(parametres);
    //        audio_parameters->setSampleFormat(codecpar->sample_fmt);
            audio_parameters->setChannelLayout(codecpar->channel_layout);
            audio_parameters->setChannels(codecpar->channels);
            audio_parameters->setSampleRate(codecpar->sample_rate);
            break;
        }
        case MediaType::MEDIA_TYPE_UNKNOWN:
            break;
        }
    }

    VideoParameters* utils::default_video_parameters(AVCodecID codec_id) {
        auto video_params = new VideoParameters;
        video_params->setCodec(codec_id, CodecType::Encoder);
        video_params->setBitrate(400000); //TODO расчитывать
        video_params->setTimeBase(DEFAULT_TIME_BASE);
        video_params->setWidth(1920);
        video_params->setHeight(1080);
        video_params->setAspectRatio({ 16, 9 });
        video_params->setFrameRate(30);
        video_params->setPixelFormat(AV_PIX_FMT_YUV420P);
        return video_params;
    }

    AudioParameters* utils::default_audio_parameters(AVCodecID codec_id) {
        auto audio_params = new AudioParameters;
        audio_params->setCodec(codec_id, CodecType::Encoder);
        audio_params->setBitrate(192000); //TODO расчитывать
        audio_params->setTimeBase(DEFAULT_TIME_BASE);
        audio_params->setSampleRate(44100);
        audio_params->setSampleFormat(audio_params->codec()->sample_fmts[0]);
        audio_params->setChannels(2);
        audio_params->setChannelLayout(uint64_t(av_get_default_channel_layout(int(audio_params->channels()))));
        return audio_params;
    }

    bool utils::rescalingRequired(StreamPair streams) {
        //TODO не реализован рескейлер
        return false;
        //
        return_if(streams.first->isAudio(),  false);
        return_if(streams.second->isAudio(), false);

        auto in = dynamic_cast<VideoParameters*>(streams.first->parameters);
        auto out = dynamic_cast<VideoParameters*>(streams.second->parameters);

        return_if(in->width()  != out->width(),  true);
        return_if(in->height() != out->height(), true);

        return false;
    }

    bool utils::resamplingRequired(StreamPair streams) {
        return_if(streams.first->isVideo(),  false); //TODO throw YException, надо ли?
        return_if(streams.second->isVideo(), false);

        auto in = dynamic_cast<AudioParameters*>(streams.first->parameters);
        auto out = dynamic_cast<AudioParameters*>(streams.second->parameters);

        return_if(in->sampleRate()      != out->sampleRate(),       true);
        return_if(in->sampleFormat()    != out->sampleFormat(),     true);
        return_if(in->channels()        != out->channels(),         true);
        return_if(in->channelLayout()   != out->channelLayout(),    true);

        return false;
    }

    bool utils::transcodingRequired(StreamPair streams) {
        auto in = streams.first->parameters;
        auto out = streams.second->parameters;

        return_if(in->codecId() != out->codecId(), true);

        return false;
    }

    Stream* utils::find_best_stream(const StreamVector& stream_list) {
        auto best_stream_it = std::max_element(stream_list.begin(), stream_list.end());
        return_if(best_stream_it == stream_list.end(), nullptr);
        return *best_stream_it;
    }

} // namespace fpp
