#include "utils.hpp"
#include <thread>
#include <atomic>

namespace fpp {

    std::string utils::media_type_to_string(MediaType value) {
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

    std::string utils::bool_to_string(bool value) {
        return value ? "true" : "false";
    }

    std::string utils::pixel_format_to_string(AVPixelFormat value) {
        const char* ret = av_get_pix_fmt_name(value);
        return_if(not_inited_ptr(ret)
                  , "NONE");
        return std::string(ret);
    }

    std::string utils::sample_format_to_string(AVSampleFormat value) {
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
        if (value == Code::ERR)                 { return true; }
        if (value == Code::EXIT)                { return true; }
        if (value == Code::EXCEPTION)           { return true; }
        if (value == Code::NOT_INITED)          { return true; }
        if (value == Code::END_OF_FILE)         { return true; }
        if (value == Code::FFMPEG_ERROR)        { return true; }
        if (value == Code::INVALID_INPUT)       { return true; }
        if (value == Code::NOT_IMPLEMENTED)     { return true; }
        if (value == Code::INVALID_CALL_ORDER)  { return true; }
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

    std::string utils::code_to_string(Code value) {
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
        return "Unknown error code: " + std::to_string(value);
    }

    std::string utils::codec_type_to_string(CodecType type) {
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

    std::string utils::rational_to_string(AVRational rational) {
        return std::to_string(rational.num)
                + "/" +
               std::to_string(rational.den);
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
//    bool utils::compatible_with_pixel_format(AVCodec* codec, AVPixelFormat pixel_format) { //TODO не работает на неоткрытом кодеке (открывать и закрывать сразу?)
//        if (codec->id != AV_CODEC_ID_H264) {
//            static_log_warning("utils", "compatible_with_pixel_format doesn't work with " << avcodec_get_name(codec->id));
//            return true;
//        }
//        AVPixelFormat h264_pxl_fmts[] = {
//            AV_PIX_FMT_YUV420P
//            , AV_PIX_FMT_YUVJ420P
//            , AV_PIX_FMT_YUV422P
//            , AV_PIX_FMT_YUVJ422P
//            , AV_PIX_FMT_YUV444P
//            , AV_PIX_FMT_YUVJ444P
//            , AV_PIX_FMT_NV12
//            , AV_PIX_FMT_NV16
//            , AV_PIX_FMT_NV21
//            , AV_PIX_FMT_YUV420P10LE
//            , AV_PIX_FMT_YUV422P10LE
//            , AV_PIX_FMT_YUV444P10LE
//            , AV_PIX_FMT_NV20LE
//            , AV_PIX_FMT_GRAY8
//            , AV_PIX_FMT_GRAY10LE
//            , AV_PIX_FMT_NONE
//        };

//        auto pix_fmt = h264_pxl_fmts;

//        while (pix_fmt[0] != AV_PIX_FMT_NONE) {
//            if (pix_fmt[0] == pixel_format) { return true; }
//            pix_fmt++;
//        }
//        return false;
//    }

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
        throw Exception("TODO mediatype_to_avmediatype");
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
            throw Exception("TODO avmt_to_mt");
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
        if (media_resurs_locator.find("anullsrc=") != std::string::npos) {  /* Silence              */
            return std::string("lavfi");
        }
        if (media_resurs_locator.find("sine=") != std::string::npos) {      /* Гул\писк             */
            return std::string("lavfi");
        }
        if (media_resurs_locator.find("video=") != std::string::npos) {     /* USB camera's video   */
            return std::string("dshow");
        }
        if (media_resurs_locator.find("audio=") != std::string::npos) {     /* USB micro's audio    */
            return std::string("TODO 13.01");
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

    ParametersPointer utils::createParams(MediaType type, ParamsType par_type) {
        switch (type) {
        case MediaType::Video:
            return std::make_shared<VideoParameters>(par_type);
        case MediaType::Audio:
            return std::make_shared<AudioParameters>(par_type);
        default:
            throw Exception("TODO createParams");
        }
    }

    Code utils::init_codecpar(AVCodecParameters* codecpar, AVCodec* codec) {
        auto codec_context = avcodec_alloc_context3(codec);
        return_if(not_inited_ptr(codec_context), Code::ERR);
        return_if(avcodec_parameters_from_context(codecpar, codec_context) < 0, Code::ERR);
        avcodec_free_context(&codec_context);
        return Code::OK;
    }

    void utils::parameters_to_context(const ParametersPointer params, AVCodecContext* codec_context) {
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
    void utils::parameters_to_avcodecpar(const ParametersPointer params, AVCodecParameters* codecpar) {
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

//    VideoParameters* utils::default_video_parameters(AVCodecID codec_id) {
//        auto video_params = new VideoParameters;
//        video_params->setCodec(codec_id, CodecType::Encoder);
//        video_params->setBitrate(400000); //TODO расчитывать
//        video_params->setTimeBase(DEFAULT_TIME_BASE);
//        video_params->setWidth(1920);
//        video_params->setHeight(1080);
//        video_params->setAspectRatio({ 16, 9 });
//        video_params->setFrameRate({ 30, 1 });
//        video_params->setPixelFormat(AV_PIX_FMT_YUV420P);
//        return video_params;
//    }

//    AudioParameters* utils::default_audio_parameters(AVCodecID codec_id) {
//        auto audio_params = new AudioParameters;
//        audio_params->setCodec(codec_id, CodecType::Encoder);
//        audio_params->setBitrate(192000); //TODO расчитывать
//        audio_params->setTimeBase(DEFAULT_TIME_BASE);
//        audio_params->setSampleRate(44100);
//        audio_params->setSampleFormat(audio_params->codec()->sample_fmts[0]);
//        audio_params->setChannels(2);
//        audio_params->setChannelLayout(uint64_t(av_get_default_channel_layout(int(audio_params->channels()))));
//        return audio_params;
//    }

    Code utils::find_encoder_for(const ParametersPointer src_prm, ParametersPointer dst_prm) { //TODO return struct { codec + type }, убрать второй аргумент 14.01
        switch (src_prm->codecId()) {
        case AVCodecID::AV_CODEC_ID_H264:
            dst_prm->setCodec("libx264");
            return Code::OK;
        default: //TODO возможны случаи с несимметричными названиями кодера и энкодера как у h264
            dst_prm->setCodec(src_prm->codecName());
            return Code::OK;
//        default:
//            return Code::INVALID_INPUT;
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
        //TODO заставляет транскодить h264 из-за несовападения YUV420P и YUVJ420P
        if (in->pixelFormat() != out->pixelFormat()) {
            static_log_warning("utils", "Rescaling required: pixel format mismatch "
                               << av_get_pix_fmt_name(in->pixelFormat()) << " != "
                               << av_get_pix_fmt_name(out->pixelFormat()));
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
                                           << rational_to_string(in->frameRate())
                                           << " != "
                                           << rational_to_string(out->frameRate())
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
        static int counter = 0;
        FILE *fDump = fopen(("debug_frames/" + std::to_string(counter++) + "_frame_yuv").c_str(), "wb");//"ab");

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
