#include "OutputFormatContext.hpp"

namespace fpp {

    OutputFormatContext::OutputFormatContext(const std::string mrl, IOType preset) :
        FormatContext(mrl, preset)
      , _output_format(nullptr)
    {
        setName("OutputFormatContext");
    }

    OutputFormatContext::~OutputFormatContext() {
        try_throw(close());
    }

    Code OutputFormatContext::init() {
        return_if(inited(), Code::INVALID_CALL_ORDER);
        try_to(createContext());
        switch (_preset) {
        case Auto: {
            try_to(guessOutputFromat());
            break;
        }
        case Event: {
            /* Video */
            auto video_params = new VideoParameters(); //TODO memory leak
            video_params->setCodec("libx264", CodecType::Encoder);
            video_params->setGopSize(2);
            video_params->setTimeBase(DEFAULT_TIME_BASE);
            try_to(createStream(video_params));
            /* Audio */
            auto audio_params = new AudioParameters(); //TODO memory leak
            audio_params->setCodec("libmp3lame", CodecType::Encoder);
            audio_params->setTimeBase(DEFAULT_TIME_BASE);
            audio_params->setSampleRate(DEFAULT_SAMPLE_RATE);
            try_to(createStream(audio_params));
            break;
        }
        case YouTube: {
            /* Video */
            auto video_parameters = new VideoParameters;
//            video_parameters->setWidth(1920);
//            video_parameters->setHeight(1080);
            video_parameters->setWidth(1280);
            video_parameters->setHeight(720);
            video_parameters->setAspectRatio({ 16,9 });
            video_parameters->setFrameRate({ 30, 1 });
            video_parameters->setBitrate(400'000);
            video_parameters->setCodec("libx264", CodecType::Encoder);
            video_parameters->setTimeBase(DEFAULT_TIME_BASE);
            video_parameters->setPixelFormat(AV_PIX_FMT_YUV420P);
            try_to(createStream(video_parameters));
//            /* Audio */
//            auto audio_parameters = new AudioParameters;
//            audio_parameters->setSampleRate(44'100);
//    //        audio_parameters->setSampleFormat(AV_SAMPLE_FMT_FLTP);
//            audio_parameters->setSampleFormat(AV_SAMPLE_FMT_FLTP);
//            audio_parameters->setBitrate(128'000);
//            audio_parameters->setChannelLayout(AV_CH_LAYOUT_STEREO);
//            audio_parameters->setChannels(2);
//            audio_parameters->setCodec("aac", CodecType::Encoder);
//    //        audio_parameters->setChannelLayout(AV_CH_LAYOUT_MONO);
//    //        audio_parameters->setChannels(1);
//    //        audio_parameters->setCodec("aac");
//            audio_parameters->setContextUid(uid());
//            try_to(createStream(audio_parameters));
            break;
        }
        case Timelapse: {
            /* Video */
            auto video_parameters = new VideoParameters;
            video_parameters->setCodec("libx264", CodecType::Encoder);
            video_parameters->setTimeBase(DEFAULT_TIME_BASE);
			video_parameters->setGopSize(2);
            try_to(createStream(video_parameters));
            break;
        }
//        case OpenCV: {
//            /* Video */
//            auto video_parameters = new VideoParameters;
//            video_parameters->setWidth(1920);
//            video_parameters->setHeight(1080);
//            video_parameters->setAspectRatio({ 16, 9 });
//            video_parameters->setFrameRate({ 22, 1 }); //TODO
//            video_parameters->setTimeBase({ 1, 1000 });
//            video_parameters->setPixelFormat(AV_PIX_FMT_BGR24);
//            video_parameters->setContextUid(uid());
//            try_to(createStream(video_parameters));
//            break;
//        }
        default:
            log_error("Invalid preset");
            break;
        }
        log_debug("Created");
        setInited(true);
        return Code::OK;
    }

    std::string OutputFormatContext::toString() const {
        /* Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/ytub-8t5w-asjj-avyf'*/
        std::string str = "Output #"
                + std::to_string(uid()) + ", "
                + utils::guess_format_short_name(_media_resource_locator) + ", " //TODO пустая строка
                + "to '" + _media_resource_locator + "'";
        return str;
    }

    Code OutputFormatContext::write(Packet packet, ReadWriteMode write_mode) {
        switch (write_mode) {
        case ReadWriteMode::Instant:
            if (av_write_frame(mediaFormatContext(), &packet.raw()) < 0) {
                log_error("Error muxing: " << packet);
                return Code::FFMPEG_ERROR;
            }
            break;
        case ReadWriteMode::Interleaved:
            if (av_interleaved_write_frame(mediaFormatContext(), &packet.raw()) < 0) {
                log_error("Error muxing: " << packet);
                return Code::FFMPEG_ERROR;
            }
            break;
        }
        return Code::OK;
    }

    Code OutputFormatContext::flush() { //TODO не потокобезопасный метод - заменить на добавление пустого пакета? тип пакета FLUSH ? для кодеков и фильтров тоже
        return_error_if(closed()
                        , "Failed to flush closed output context"
                        , Code::INVALID_CALL_ORDER);
        return_error_if(av_write_frame(mediaFormatContext(), nullptr) < 0
                        , "Flush failed"
                        , Code::FFMPEG_ERROR);
        return Code::OK;
    }

    Code OutputFormatContext::createContext() {
        return_error_if(inited_ptr(_format_context)
                        , "Context already created!"
                        , Code::INVALID_CALL_ORDER);
        _format_context = avformat_alloc_context();
        std::string format_short_name = utils::guess_format_short_name(_media_resource_locator);
        const char* format_name = format_short_name.empty() ? nullptr : format_short_name.c_str();
        if (avformat_alloc_output_context2(&_format_context, nullptr, format_name, _media_resource_locator.c_str()) < 0) {
            log_error("Failed to alloc output context.");
            return Code::ERR;
        }
        return Code::OK;
    }

    Code OutputFormatContext::openContext() {
        return_if_not(inited(), Code::NOT_INITED);
        return_error_if(_streams.empty()
                        , "No streams to mux were specified: " << _media_resource_locator
                        , Code::NOT_INITED);
        for (auto&& avstream : streams()) { //TODO refactoring
            if (not_inited_codec_id(avstream->raw()->codecpar->codec_id)) {
                utils::parameters_to_avcodecpar(avstream->params, avstream->raw()->codecpar);
            }
            try_to(avstream->init());
        }
        if (!(_format_context->flags & AVFMT_NOFILE)) {
            if (avio_open(&_format_context->pb, _media_resource_locator.c_str(), AVIO_FLAG_WRITE) < 0) {
                log_error("Could not open output: " << _media_resource_locator);
                return Code::INVALID_INPUT;
            }
        }
//        auto debug_valu1 = _format_context->streams[0]->codecpar;
//        auto debug_value = _format_context->streams[1]->codecpar;
//        log_warning("---> OUT " << streams()[1]->params->toString());
        if (auto ret = avformat_write_header(_format_context, nullptr); ret < 0) {
            log_error("Error occurred when opening output: " << _media_resource_locator << ", " << ret);
            return Code::ERR;
        }
        {
            av_dump_format(_format_context, 0, _media_resource_locator.c_str(), 1);
            return Code::OK;
        }
    }

    Code OutputFormatContext::closeContext() {
        return_if(closed(), Code::OK);
        if (av_write_trailer(_format_context) != 0) {
            log_error("Failed to write the stream trailer to an output media file");
            return Code::ERR;
        }
        avio_close(_format_context->pb);
        return Code::OK;
    }

    Code OutputFormatContext::guessOutputFromat() {// см: _format_context->oformat
        AVOutputFormat* output_format = av_guess_format(nullptr, _media_resource_locator.c_str(), nullptr);
        if (output_format == nullptr) {
            log_error("Failed guess output format: " << _media_resource_locator);
            return Code::INVALID_INPUT;
        }
        _output_format = output_format;
        return Code::OK;
    }

    Code OutputFormatContext::parseOutputFormat() {
        return_if_not(inited_ptr(_output_format), Code::NOT_INITED);
        {
            //TODO:                                   ↓ mp3 AVOutputFormat дает видеокодек PNG ↓
            auto& video_codec_id = _output_format->video_codec;
            if (inited_codec_id(video_codec_id) && _output_format->video_codec != AV_CODEC_ID_PNG) {
                try_to(createStream(utils::default_video_parameters(video_codec_id)));
            }
        }
        {
            auto& audio_codec_id = _output_format->audio_codec;
            if (inited_codec_id(audio_codec_id)) {
                try_to(createStream(utils::default_audio_parameters(audio_codec_id)));
            }
        }
        return Code::OK;
    }

} // namespace fpp
