#include "Utils.hpp"
#include <thread>
#include <atomic>
#include <algorithm>

extern "C" {
    #include <libavutil/imgutils.h>
}

namespace fpp {

    std::string utils::to_string(MediaType value) {
        switch (value) {
        case MediaType::Unknown:
            return "Unknown";
        case MediaType::Video:
            return "Video";
        case MediaType::Audio:
            return "Audio";
        case MediaType::EndOF:
            return "EOF";
        }
        return "Invalid";
    }

    std::string utils::pts_to_string(int64_t pts) {
        return pts == AV_NOPTS_VALUE ? "NOPTS" : std::to_string(pts);
    }

    std::string utils::to_string(bool value) {
        return value ? "true" : "false";
    }

    std::string utils::to_string(AVPixelFormat value) {
        const char* ret = av_get_pix_fmt_name(value);
        return_if(not_inited_ptr(ret)
                  , "NONE");
        return std::string(ret);
    }

    std::string utils::to_string(AVSampleFormat value) {
        const char* ret = av_get_sample_fmt_name(value);
        return_if(not_inited_ptr(ret)
                  , "NONE");
        return std::string(ret);
    }

    void utils::sleep_for(int64_t milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    void utils::sleep_for_ms(int64_t milliseconds) {
        sleep_for(milliseconds);
    }

    void utils::sleep_for_sec(int64_t seconds) {
        sleep_for_ms(seconds * 1'000);
    }

    void utils::sleep_for_min(int64_t minutes) {
        sleep_for_sec(minutes * 60);
    }

    std::string utils::time_to_string(int64_t time_stamp, AVRational time_base) {
        const auto time_ms = av_rescale_q(time_stamp, time_base, DEFAULT_TIME_BASE);
        const int64_t ms = time_ms % 1000;
        const int64_t ss = (time_ms / 1000) % 60;
        const int64_t mm = ((time_ms / 1000) % 3600) / 60;
        const int64_t hh = (time_ms / 1000) / 3600;
        return std::to_string(hh) + ':' + std::to_string(mm) + ':' + std::to_string(ss) + '.' + std::to_string(ms);
    }

    bool utils::exit_code(Code value) {
        if (error_code(value))                  { return true; }
        if (value == Code::EXIT)                { return true; }
        if (value == Code::END_OF_FILE)         { return true; }
        return false;
    }

    bool utils::error_code(Code value) {
        if (value == Code::ERR)                 { return true; }
        if (value == Code::EXCEPTION)           { return true; }
        if (value == Code::NOT_INITED)          { return true; }
        if (value == Code::FFMPEG_ERROR)        { return true; }
        if (value == Code::INVALID_INPUT)       { return true; }
        if (value == Code::NOT_IMPLEMENTED)     { return true; }
        if (value == Code::INVALID_CALL_ORDER)  { return true; }
        return false;
    }

    std::string utils::to_string(Code value) {
        if (value == Code::OK)                  { return "OK";                      }
        if (value == Code::ERR)                 { return "Error";                   }
        if (value == Code::EXIT)                { return "Exit";                    }
        if (value == Code::AGAIN)               { return "Again";                   }
        if (value == Code::NOT_INITED)          { return "Not inited";              }
        if (value == Code::END_OF_FILE)         { return "EOF";                     }
        if (value == Code::FFMPEG_ERROR)        { return "FFmpeg error";            }
        if (value == Code::INVALID_INPUT)       { return "Invalid input";           }
        if (value == Code::NOT_IMPLEMENTED)     { return "Method not implemented";  }
        if (value == Code::INVALID_CALL_ORDER)  { return "Invalid call order";      }
        return "Unknown error code: " + std::to_string(int(value));
    }

    std::string utils::to_string(CodecType type) {
        switch (type) {
        case CodecType::Unknown:
            return "Unknown";
        case CodecType::Decoder:
            return "Decoder";
        case CodecType::Encoder:
            return "Encoder";
        }
        return "Error";
    }

    std::string utils::to_string(AVRational rational) {
        return std::to_string(rational.num)
                + "/" +
                std::to_string(rational.den);
    }

    SharedVideoParameters utils::makeVideoParams(SharedParameters params) {
        return std::static_pointer_cast<VideoParameters>(params);
    }

    SharedAudioParameters utils::makeAudioParams(SharedParameters params) {
        return std::static_pointer_cast<AudioParameters>(params);
    }

    bool utils::compatible_with_pixel_format(const AVCodec* codec, AVPixelFormat pixel_format) {
        if (not_inited_ptr(codec->pix_fmts)) {
            static_log_warning("utils", "compatible_with_pixel_format failed: codec->pix_fmts is NULL");
            return true;
        }

        auto pix_fmt = codec->pix_fmts;
        while (pix_fmt[0] != AV_PIX_FMT_NONE) {
            if (pix_fmt[0] == pixel_format) { return true; }
            pix_fmt++;
        }
        return false;
    }

    bool utils::compatible_with_sample_format(const AVCodec* codec, AVSampleFormat sample_format) {
        if (not_inited_ptr(codec->sample_fmts)) {
            static_log_warning("utils", "compatible_with_sample_format failed: codec->sample_fmts is NULL");
            return true;
        }

        auto smp_fmt = codec->sample_fmts;
        while (smp_fmt[0] != AV_SAMPLE_FMT_NONE) {
            if (smp_fmt[0] == sample_format) { return true; }
            smp_fmt++;
        }
        return false;
    }

    AVMediaType utils::mediatype_to_avmediatype(MediaType media_type) {
        switch (media_type) {
        case MediaType::Unknown:
            return AVMediaType::AVMEDIA_TYPE_UNKNOWN;
        case MediaType::Video:
            return AVMediaType::AVMEDIA_TYPE_VIDEO;
        case MediaType::Audio:
            return AVMediaType::AVMEDIA_TYPE_AUDIO;
        case MediaType::EndOF:
            return AVMEDIA_TYPE_DATA;
        }
        throw std::invalid_argument("mediatype_to_avmediatype failed: " + std::to_string(int(media_type)));
    }

    MediaType utils::avmt_to_mt(AVMediaType avmedia_type) {
        switch (avmedia_type) {
        case AVMediaType::AVMEDIA_TYPE_UNKNOWN:
            return MediaType::Unknown;
        case AVMediaType::AVMEDIA_TYPE_VIDEO:
            return MediaType::Video;
        case AVMediaType::AVMEDIA_TYPE_AUDIO:
            return MediaType::Audio;
        default:
            throw FFmpegException("TODO avmt_to_mt");
        }
    }

    uid_t utils::gen_uid() {
        static std::atomic<uid_t> object_uid_handle = 0;
        return object_uid_handle++;
    }

    uid_t utils::gen_stream_uid(uid_t context_uid, uid_t stream_index) {
        return (context_uid + 1) * 100 + stream_index;
    }

    uid_t utils::get_context_uid(uid_t stream_uid) {
        return stream_uid / 100;
    }

    const char* utils::guess_format_short_name(std::string media_resurs_locator) {
        if (media_resurs_locator.find("rtsp://") != std::string::npos) {
            return "rtsp";
        }
        if (media_resurs_locator.find("rtmp://") != std::string::npos) {
            return "flv";
        }
        if (media_resurs_locator.find("aevalsrc=") != std::string::npos) {
            return "lavfi";
        }
        if (media_resurs_locator.find("anullsrc=") != std::string::npos) {  /* Silence              */
            return "lavfi";
        }
        if (media_resurs_locator.find("sine=") != std::string::npos) {      /* Гул\писк             */
            return "lavfi";
        }
        if (media_resurs_locator.find("video=") != std::string::npos) {     /* USB camera's video   */
            return "dshow";
        }
        if (media_resurs_locator.find("audio=") != std::string::npos) {     /* USB micro's audio    */
            return "TODO 13.01";
        }
        return nullptr;
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

    SharedParameters utils::createParams(MediaType type) {
        switch (type) {
        case MediaType::Video:
            return std::make_shared<VideoParameters>();
        case MediaType::Audio:
            return std::make_shared<AudioParameters>();
        default:
            throw std::invalid_argument("createParams failed");
        }
    }

    Code utils::init_codecpar(AVCodecParameters* codecpar, AVCodec* codec) {
        auto codec_context = avcodec_alloc_context3(codec);
        return_if(not_inited_ptr(codec_context), Code::ERR);
        return_if(avcodec_parameters_from_context(codecpar, codec_context) < 0, Code::ERR);
        avcodec_free_context(&codec_context);
        return Code::OK;
    }

    void utils::parameters_to_context(const SharedParameters params, AVCodecContext* codec_context) {
        codec_context->codec_id = params->codecId();
        codec_context->bit_rate = params->bitrate();
//        codec->time_base = param->timeBase();
//        codec_context->time_base = { 1, 16000 /*тут нужен таймбейс входного потока, либо рескейлить в энкодере*/ };

        switch (params->type()) {
        case MediaType::Video: {
            auto video_parameters = static_cast<const VideoParameters*>(params.get());
            codec_context->pix_fmt      = video_parameters->pixelFormat();
            codec_context->width        = int(video_parameters->width());
            codec_context->height       = int(video_parameters->height());
//            codec->time_base    = param->timeBase();
            codec_context->framerate    = video_parameters->frameRate();
            codec_context->gop_size     = int(video_parameters->gopSize());

            //TODO опции не применяются, лог раздражает
//            if (auto ret = av_opt_set(codec->priv_data, "crf", "23", 0); ret != 0) {
//                static_log_warning("utils", "av_opt_set crf     failed: " << ret /*<< " - " << av_err2str(ret)*/);
//            }
//            if (auto ret = av_opt_set(codec->priv_data, "preset", "ultrafast", 0); ret != 0) {
//                static_log_warning("utils", "av_opt_set preset  failed: " << ret /*<< " - " << av_err2str(ret)*/);
//            }
//            if (auto ret = av_opt_set(codec->priv_data, "tune", "zerolatency", 0); ret != 0) {
//                static_log_warning("utils", "av_opt_set tune    failed: " << ret /*<< " - " << av_err2str(ret)*/);
//            }
//            if (auto ret = av_opt_set(codec->priv_data, "threads", "0", 0); ret != 0) {
//                static_log_warning("utils", "av_opt_set threads failed: " << ret /*<< " - " << av_err2str(ret)*/);
//            }

    //        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
            break;
        }
        case MediaType::Audio: {
            auto audio_parameters = static_cast<const AudioParameters*>(params.get());
            codec_context->sample_fmt       = audio_parameters->sampleFormat();
            codec_context->channel_layout   = audio_parameters->channelLayout();
            codec_context->channels         = int(audio_parameters->channels());
            codec_context->sample_rate      = int(audio_parameters->sampleRate());
            break;
        }
        case MediaType::Unknown:
            break;
        }
    }

    //TODO
    void utils::parameters_to_avcodecpar(const SharedParameters params, AVCodecParameters* codecpar) {
        codecpar->codec_id = params->codecId();
        codecpar->bit_rate = params->bitrate();

        switch (params->type()) {
        case MediaType::Video: {
            codecpar->codec_type = AVMediaType::AVMEDIA_TYPE_VIDEO;
            auto video_parameters = dynamic_cast<VideoParameters*>(params.get());
            codecpar->width                  = int(video_parameters->width());
            codecpar->height                 = int(video_parameters->height());
    //        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
            codecpar->format                = int(video_parameters->pixelFormat());
            break;
        }
        case MediaType::Audio: {
            codecpar->codec_type = AVMediaType::AVMEDIA_TYPE_AUDIO;
            auto audio_parameters = dynamic_cast<AudioParameters*>(params.get());
            codecpar->channel_layout   = audio_parameters->channelLayout();
            codecpar->channels         = int(audio_parameters->channels());
            codecpar->sample_rate      = int(audio_parameters->sampleRate());
            break;
        }
        case MediaType::Unknown:
            break;
        }
    }

    //TODO
    void utils::parameters_from_avcodecpar(Parameters* parametres, AVCodecParameters* codecpar) {
//        parametres->setCodec(codecpar->codec_id);
        parametres->setBitrate(codecpar->bit_rate);

        switch (parametres->type()) {
        case MediaType::Video: {
            auto video_parameters = dynamic_cast<VideoParameters*>(parametres);
    //        video_parameters->setPixelFormat(codecpar->pix_fmt);
            video_parameters->setWidth(codecpar->width);
            video_parameters->setHeight(codecpar->height);
    //        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
            break;
        }
        case MediaType::Audio: {
            auto audio_parameters = dynamic_cast<AudioParameters*>(parametres);
    //        audio_parameters->setSampleFormat(codecpar->sample_fmt);
            audio_parameters->setChannelLayout(codecpar->channel_layout);
            audio_parameters->setChannels(codecpar->channels);
            audio_parameters->setSampleRate(codecpar->sample_rate);
            break;
        }
        case MediaType::Unknown:
            break;
        }
    }

    MediaType utils::antitype(const MediaType value) {
        switch (value) {
        case MediaType::Video:
            return MediaType::Audio;
        case MediaType::Audio:
            return MediaType::Video;
        default:
            throw std::exception("Bad media type - antitype()");
        }
    }

    bool utils::rescaling_required(const IOParams params) {
        return_if(params.in->isAudio(),  false);
        return_if(params.out->isAudio(), false);

        auto in = static_cast<const VideoParameters * const>(params.in.get());
        auto out = static_cast<const VideoParameters * const>(params.out.get());

        if (in->width() != out->width()) {
            static_log_warning("utils", "Rescaling required: width mismatch "
                               << in->width() << " != " << out->width());
            return true;
        }
        if (in->height() != out->height()) {
            static_log_warning("utils", "Rescaling required: height mismatch "
                               << in->height() << " != " << out->height());
            return true;
        }
        if (in->pixelFormat() != out->pixelFormat()) {
            static_log_warning("utils", "Rescaling required: pixel format mismatch "
                               << in->pixelFormat() << " != " << out->pixelFormat());
            return true;
        }

        return false;
    }

    bool utils::resampling_required(const IOParams params) {
        return_if(params.in->isVideo(),  false);
        return_if(params.out->isVideo(), false);

        auto in = static_cast<const AudioParameters * const>(params.in.get());
        auto out = static_cast<const AudioParameters * const>(params.out.get());

        if (in->sampleRate() != out->sampleRate()) {
            static_log_warning("utils", "Resampling required: sample rate mismatch "
                               << in->sampleRate() << " != " << out->sampleRate());
            return true;
        }
        if (in->sampleFormat() != out->sampleFormat()) {
            static_log_warning("utils", "Resampling required: sample format mismatch "
                               << in->sampleFormat() << " != " << out->sampleFormat());
            return true;
        }
        if (in->channels() != out->channels()) {
            static_log_warning("utils", "Resampling required: channels mismatch "
                               << in->channels() << " != " << out->channels());
            return true;
        }
        if (in->channelLayout() != out->channelLayout()) {
            static_log_warning("utils", "Resampling required: channel layout mismatch "
                               << in->channelLayout() << " != " << out->channelLayout());
            return true;
        }

        return false;
    }

    bool utils::video_filter_required(const IOParams params) {
        return_if(params.in->isAudio(),  false);
        return_if(params.out->isAudio(), false);

        auto in = static_cast<const VideoParameters*>(params.in.get());
        auto out = static_cast<const VideoParameters*>(params.out.get());

//        if (av_cmp_q(in->frameRate(), out->frameRate()) != 0) {
        if (in->frameRate() != out->frameRate()) {
            static_log_warning("utils", "Video filter required: framerate mismatch "
                                           << to_string(in->frameRate())
                                           << " != "
                                           << to_string(out->frameRate())
            );
            return true;
        }

        return false;
    }

    bool utils::audio_filter_required(const IOParams params) {
        return false;
    }

    bool utils::transcoding_required(const IOParams params) {
        auto in = params.in;
        auto out = params.out;

        if (in->codecId() != out->codecId()) {
            static_log_warning("utils", "Transcoding required: codec id mismatch "
                                           << in->codecId()
                                           << " != "
                                           << out->codecId()
            );
            return true;
        }

        return false;
    }

    SharedStream utils::find_best_stream(const StreamVector& stream_list) {
        auto best_stream_it = std::max_element(std::begin(stream_list), std::end(stream_list)
            , [](const auto& lhs, const auto& rhs) {
            return lhs->params->betterThen(rhs->params);
        });
        return_if(best_stream_it == stream_list.end(), nullptr);
        return *best_stream_it;
    }

    bool utils::compare_float(float a, float b) {
        const float epsilon = 0.0001f;
        return fabs(a - b) < epsilon;
    }

    bool utils::equal_rational(AVRational a, AVRational b) {
        return av_cmp_q(a, b) == 0;
    }

} // namespace fpp
