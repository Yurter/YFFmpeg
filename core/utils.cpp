#include "utils.hpp"
//extern "C" {
//#include <libavutil/rational.h>
//}
#include <thread>

#define FLOAT_EPSILON 0.0001f

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

    std::string utils::bool_to_string(bool value) {
        return value ? "true" : "false";
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

    bool utils::exit_code(Code code) {
        if (code == Code::ERR)             { return true; }
        if (code == Code::EXIT)            { return true; }
        if (code == Code::EXCEPTION)       { return true; }
        if (code == Code::NOT_INITED)      { return true; }
        if (code == Code::END_OF_FILE)     { return true; }
        if (code == Code::FFMPEG_ERROR)    { return true; }
        if (code == Code::INVALID_INPUT)   { return true; }
        if (code == Code::NOT_IMPLEMENTED) { return true; }
        return false;
    }

    bool utils::error_code(Code code) {
        if (code == Code::ERR)             { return true; }
        if (code == Code::EXCEPTION)       { return true; }
        if (code == Code::NOT_INITED)      { return true; }
        if (code == Code::FFMPEG_ERROR)    { return true; }
        if (code == Code::INVALID_INPUT)   { return true; }
        if (code == Code::NOT_IMPLEMENTED) { return true; }
        return false;
    }

    std::string utils::code_to_string(Code code) {
        if (code == Code::OK)               { return "OK";              }
        if (code == Code::ERR)              { return "Error";           }
        if (code == Code::EXIT)             { return "Exit";            }
        if (code == Code::AGAIN)            { return "Again";           }
        if (code == Code::NOT_INITED)       { return "Not inited";      }
        if (code == Code::END_OF_FILE)      { return "EOF";             }
        if (code == Code::FFMPEG_ERROR)     { return "FFmpeg error";    }
        if (code == Code::INVALID_INPUT)    { return "Invalid input";   }
        if (code == Code::NOT_IMPLEMENTED)  { return "Method not implemented"; }
        return "Unknown error code: " + std::to_string(code);
    }

    std::string utils::codec_type_to_string(CodecType type) {
        switch (type) {
        case CodecType::Decoder:
            return "Decoder";
        case CodecType::Encoder:
            return "Encoder";
        }
        return "Unknown";
    }

    std::string utils::rational_to_string(AVRational rational) {
        std::string str = std::to_string(rational.num)
                            + "/"
                            + std::to_string(rational.den);
        return str;
    }

    bool utils::compatible_with_pixel_format(AVCodec* codec, AVPixelFormat pixel_format) {
        AVPixelFormat h264_pxl_fmts[] = {
            AV_PIX_FMT_YUV420P
            , AV_PIX_FMT_YUVJ420P
            , AV_PIX_FMT_YUV422P
            , AV_PIX_FMT_YUVJ422P
            , AV_PIX_FMT_YUV444P
            , AV_PIX_FMT_YUVJ444P
            , AV_PIX_FMT_NV12
            , AV_PIX_FMT_NV16
            , AV_PIX_FMT_NV21
            , AV_PIX_FMT_YUV420P10LE
            , AV_PIX_FMT_YUV422P10LE
            , AV_PIX_FMT_YUV444P10LE
            , AV_PIX_FMT_NV20LE
            , AV_PIX_FMT_GRAY8
            , AV_PIX_FMT_GRAY10LE
        };

        auto pix_fmt = codec->pix_fmts;

        if (not_inited_ptr(pix_fmt)) {
            static_log_error("utils", codec->name << " doesn't contain any default pixel format");

            if (codec->id == AV_CODEC_ID_H264) {
                static_log_error("utils", "Using hardcoded pix_fmts");
                pix_fmt = h264_pxl_fmts;
            } else {
                static_log_error("utils", "Don't know what to do :(");
                return true;
            }
        }
        while (pix_fmt[0] != AV_PIX_FMT_NONE) {
            if (pix_fmt[0] == pixel_format) { return true; }
            pix_fmt++;
        }
        return false;
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
        if (media_resurs_locator.find("video=") != std::string::npos) {
            return std::string("dshow");
        }
        return std::string(); //TODO use fmpeg funtion to get format list, нет нельзя: нужно возвращать пустую строку для логики в месте вызова
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
            codec->time_base    = parametres->timeBase();
            codec->framerate    = video_parameters->frameRate();

            if (auto ret = av_opt_set(codec->priv_data, "crf", "23", 0); ret != 0) {
                static_log_warning("utils", "av_opt_set crf     failed: " << ret /*<< " - " << av_err2str(ret)*/);
            }
            if (auto ret = av_opt_set(codec->priv_data, "preset", "ultrafast", 0); ret != 0) {
                static_log_warning("utils", "av_opt_set preset  failed: " << ret /*<< " - " << av_err2str(ret)*/);
            }
            if (auto ret = av_opt_set(codec->priv_data, "tune", "zerolatency", 0); ret != 0) {
                static_log_warning("utils", "av_opt_set tune    failed: " << ret /*<< " - " << av_err2str(ret)*/);
            }
            if (auto ret = av_opt_set(codec->priv_data, "threads", "0", 0); ret != 0) {
                static_log_warning("utils", "av_opt_set threads failed: " << ret /*<< " - " << av_err2str(ret)*/);
            }

//            if (auto ret = av_opt_set(codec->priv_data, "crf", "23", 0); ret != 0) {
//                static_log_warning("utils", "av_opt_set crf failed: " << ret << " - " << av_err2str(ret));
//            }
//            if (auto ret = av_opt_set(codec->priv_data, "preset", "ultrafast", 0); ret != 0) {
//                static_log_warning("utils", "av_opt_set preset failed: " << ret << " - " << av_err2str(ret));
//            }
//            if (auto ret = av_opt_set(codec->priv_data, "tune", "zerolatency", 0); ret != 0) {
//                static_log_warning("utils", "av_opt_set tune failed: " << ret << " - " << av_err2str(ret));
//            }
//            if (auto ret = av_opt_set(codec->priv_data, "threads", "0", 0); ret != 0) {
//                static_log_warning("utils", "av_opt_set threads failed: " << ret << " - " << av_err2str(ret));
//            }

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
            codecpar->codec_type = AVMediaType::AVMEDIA_TYPE_VIDEO;
            auto video_parameters = dynamic_cast<VideoParameters*>(parametres);
            codecpar->width                  = int(video_parameters->width());
            codecpar->height                 = int(video_parameters->height());
    //        codec->sample_aspect_ratio    = video_parameters->sampl; //TODO
            codecpar->format                = int(video_parameters->pixelFormat());
            break;
        }
        case MediaType::MEDIA_TYPE_AUDIO: {
            codecpar->codec_type = AVMediaType::AVMEDIA_TYPE_AUDIO;
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
//        parametres->setCodec(codecpar->codec_id);
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
        video_params->setFrameRate({ 30, 1 });
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

    Code utils::find_encoder_for(const Parameters * const src_prm, Parameters * const dst_prm) {
        switch (src_prm->codecId()) {
        case AVCodecID::AV_CODEC_ID_H264:
            dst_prm->setCodec("libx264", CodecType::Encoder);
            return Code::OK;
        default: //TODO возможны случаи с несимметричными названиями кодера и энкодера как у h264
            dst_prm->setCodec(src_prm->codecName(), CodecType::Encoder);
            return Code::OK;
//        default:
//            return Code::INVALID_INPUT;
        }
    }

    bool utils::rescaling_required(const StreamPair streams) {
        return_if(streams.first->isAudio(),  false);
        return_if(streams.second->isAudio(), false);

        auto in = static_cast<VideoParameters*>(streams.first->parameters);
        auto out = static_cast<VideoParameters*>(streams.second->parameters);

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
        //TODO заставляет транскодить h264 из-за несовападения YUV420P и YUVJ420P
        if (in->pixelFormat() != out->pixelFormat()) {
            static_log_warning("utils", "Rescaling required: pixel format mismatch "
                               << av_get_pix_fmt_name(in->pixelFormat()) << " != "
                               << av_get_pix_fmt_name(out->pixelFormat()));
            return true;
        }

//        return_if(in->width()  != out->width(),  true);
//        return_if(in->height() != out->height(), true);
//        return_if(in->pixelFormat() != out->pixelFormat(),  true);

        return false;
    }

    bool utils::resampling_required(const StreamPair streams) {
        return_if(streams.first->isVideo(),  false);
        return_if(streams.second->isVideo(), false);

        auto in = static_cast<AudioParameters*>(streams.first->parameters);
        auto out = static_cast<AudioParameters*>(streams.second->parameters);

        return_if(in->sampleRate()      != out->sampleRate(),       true);
        return_if(in->sampleFormat()    != out->sampleFormat(),     true);
        return_if(in->channels()        != out->channels(),         true);
        return_if(in->channelLayout()   != out->channelLayout(),    true);

        return false;
    }

    bool utils::video_filter_required(const StreamPair streams) {
        return_if(streams.first->isAudio(),  false);
        return_if(streams.second->isAudio(), false);

        auto in = dynamic_cast<VideoParameters*>(streams.first->parameters);
        auto out = dynamic_cast<VideoParameters*>(streams.second->parameters);

//        return_if(!compare_float(in->frameRate(), out->frameRate()),  true);
        return_if(av_cmp_q(in->frameRate(), out->frameRate()) != 0, true);

        return false;
    }

    bool utils::audio_filter_required(const StreamPair streams) {
        return false;
    }

    bool utils::transcoding_required(const StreamPair streams) {
        auto in = streams.first->parameters;
        auto out = streams.second->parameters;

        return_if(in->codecId() != out->codecId(), true);
//        return_if(in->codecName() != out->codecName(), true);

        return false;
    }

    Stream* utils::find_best_stream(const StreamVector& stream_list) {
        auto best_stream_it = std::max_element(stream_list.begin(), stream_list.end());
        return_if(best_stream_it == stream_list.end(), nullptr);
        return *best_stream_it;
    }

    bool utils::compare_float(float a, float b) {
        return fabs(a - b) < FLOAT_EPSILON;
    }

    int utils::save_frame_as_jpeg(AVCodecContext* pCodecCtx, AVFrame* pFrame, int FrameNo) {
        AVCodec *jpegCodec = avcodec_find_encoder(AV_CODEC_ID_JPEG2000);
        if (!jpegCodec) {
            return -1;
        }
        AVCodecContext *jpegContext = avcodec_alloc_context3(jpegCodec);
        if (!jpegContext) {
            return -1;
        }

        jpegContext->pix_fmt = pCodecCtx->pix_fmt;
        jpegContext->height = pFrame->height;
        jpegContext->width = pFrame->width;

        if (avcodec_open2(jpegContext, jpegCodec, NULL) < 0) {
            return -1;
        }
        FILE *JPEGFile;
        char JPEGFName[256];

//        AVPacket packet = {.data = NULL, .size = 0};
        AVPacket packet;
        packet.data = NULL;
        packet.size = 0;
        av_init_packet(&packet);
        int gotFrame;

        if (avcodec_encode_video2(jpegContext, &packet, pFrame, &gotFrame) < 0) {
            return -1;
        }

        sprintf(JPEGFName, "dvr-%06d.jpg", FrameNo);
        JPEGFile = fopen(JPEGFName, "wb");
        fwrite(packet.data, 1, packet.size, JPEGFile);
        fclose(JPEGFile);

        av_free_packet(&packet);
        avcodec_close(jpegContext);
        return 0;
    }

    void utils::SaveAvFrame(AVFrame *avFrame)
    {
        FILE *fDump = fopen("frame_yuv", "ab");

        uint32_t pitchY = avFrame->linesize[0];
        uint32_t pitchU = avFrame->linesize[1];
        uint32_t pitchV = avFrame->linesize[2];

        uint8_t *avY = avFrame->data[0];
        uint8_t *avU = avFrame->data[1];
        uint8_t *avV = avFrame->data[2];

        for (uint32_t i = 0; i < avFrame->height; i++) {
            fwrite(avY, avFrame->width, 1, fDump);
            avY += pitchY;
        }

        for (uint32_t i = 0; i < avFrame->height/2; i++) {
            fwrite(avU, avFrame->width/2, 1, fDump);
            avU += pitchU;
        }

        for (uint32_t i = 0; i < avFrame->height/2; i++) {
            fwrite(avV, avFrame->width/2, 1, fDump);
            avV += pitchV;
        }

        fclose(fDump);
    }

} // namespace fpp
