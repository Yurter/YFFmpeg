#include "OutputFormatContext.hpp"

namespace fpp {

    OutputFormatContext::OutputFormatContext(const std::string& mrl, IOPreset preset)
        : FormatContext { mrl, preset }
        , _output_format { nullptr } {
        setName("OutFmtCtx");
    }

    OutputFormatContext::~OutputFormatContext() {
        try_throw(close());
    }

    Code OutputFormatContext::init() { //TODO refactoring 14.01
        return_if(inited(), Code::OK);
        try_to(createContext());
        switch (preset()) {
        case IOPreset::Auto: {
            try_to(guessOutputFromat());
            break;
        }
        case IOPreset::Event: {
            StreamVector stream_list;
            /* Video */
            auto video_params = std::make_shared<VideoParameters>(ParamsType::Output);
            video_params->setCodec("libx264");
            video_params->setGopSize(2);
            video_params->setTimeBase(DEFAULT_TIME_BASE);
            stream_list.push_back(createStream(video_params));
            /* Audio */
//            auto audio_params = std::make_shared<AudioParameters>(ParamsType::Output);
//            audio_params->setCodec("libmp3lame");
//            audio_params->setTimeBase(DEFAULT_TIME_BASE);
//            audio_params->setSampleRate(DEFAULT_SAMPLE_RATE);
//            stream_list.push_back(createStream(audio_params));
            setStreams(stream_list);
            break;
        }
        case IOPreset::YouTube: {
            /* Video */
//            auto video_parameters = std::make_shared<VideoParameters>();
////            video_parameters->setWidth(1920);
////            video_parameters->setHeight(1080);
//            video_parameters->setWidth(1280);
//            video_parameters->setHeight(720);
//            video_parameters->setAspectRatio({ 16,9 });
//            video_parameters->setFrameRate({ 30, 1 });
//            video_parameters->setBitrate(400'000);
//            video_parameters->setCodec("libx264", CodecType::Encoder);
//            video_parameters->setTimeBase(DEFAULT_TIME_BASE);
//            video_parameters->setPixelFormat(AV_PIX_FMT_YUV420P);
//            try_to(createStream(video_parameters));
//            /* Audio */
//            auto audio_parameters = new AudioParameters; //TODO memory leak 14.01
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
        case IOPreset::Timelapse: {
            StreamVector stream_list;
            /* Video */
            auto video_parameters = std::make_shared<VideoParameters>(ParamsType::Output);
            video_parameters->setCodec("libx264");
            video_parameters->setTimeBase(DEFAULT_TIME_BASE);
			video_parameters->setGopSize(2);
            stream_list.push_back(createStream(video_parameters));
            setStreams(stream_list);
            break;
        }
//        case IOPreset::OpenCV: {
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

    std::string OutputFormatContext::toString() const { //TODO реализация 14.01
        /* Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/ytub-8t5w-asjj-avyf'*/
        return "Output #"
                + utils::guess_format_short_name(mediaResourceLocator()) + ", " //TODO пустая строка
                + "to '" + mediaResourceLocator() + "'";
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
        return_error_if(inited_ptr(mediaFormatContext())
                        , "Context already created!"
                        , Code::INVALID_CALL_ORDER);
        try_to(setMediaFormatContext(avformat_alloc_context()));
        std::string format_short_name = utils::guess_format_short_name(mediaResourceLocator());
        const char* format_name = format_short_name.empty() ? nullptr : format_short_name.c_str();
        if (avformat_alloc_output_context2(mediaFormatContext2(), nullptr, format_name, mediaResourceLocator().c_str()) < 0) {
            log_error("Failed to alloc output context.");
            return Code::ERR;
        }
        return Code::OK;
    }

    Code OutputFormatContext::openContext() {
        return_if(opened(), Code::OK);
        return_if_not(inited(), Code::NOT_INITED);
//        return_error_if(streams().empty() //TODO убедиться в необходимости этого кода 16.01
//                        , "No streams to mux were specified: " << mediaResourceLocator()
//                        , Code::NOT_INITED);
//        for (auto&& avstream : streams()) { //TODO refactoring
//            if (not_inited_codec_id(avstream->raw()->codecpar->codec_id)) {
//                utils::parameters_to_avcodecpar(avstream->params, avstream->raw()->codecpar);
//            }
//            try_to(avstream->init());
//        }
//        log_error(">> " << mediaFormatContext()->streams[0]->codecpar->codec_id << " " << mediaFormatContext()->streams[1]->codecpar->codec_id);
        if (!(mediaFormatContext()->flags & AVFMT_NOFILE)) {
            if (auto ret = avio_open(&mediaFormatContext()->pb, mediaResourceLocator().c_str(), AVIO_FLAG_WRITE); ret < 0) {
                log_error("Could not open output: " << mediaResourceLocator());
                return Code::INVALID_INPUT;
            }
//            if (avio_open(&mediaFormatContext()->pb, mediaResourceLocator().c_str(), AVIO_FLAG_WRITE) < 0) {
//                log_error("Could not open output: " << mediaResourceLocator());
//                return Code::INVALID_INPUT;
//            }
        }
        if (auto ret = avformat_write_header(mediaFormatContext(), nullptr); ret < 0) { //TODO unesed result 14.01
            log_error("Error occurred when opening output: " << mediaResourceLocator() << ", " << ret);
            return Code::ERR;
        }
        return Code::OK;
    }

    Code OutputFormatContext::closeContext() {
        return_if(closed(), Code::OK);
        if (av_write_trailer(mediaFormatContext()) != 0) {
            log_error("Failed to write the stream trailer to an output media file");
            return Code::ERR;
        }
        avio_close(mediaFormatContext()->pb);
        return Code::OK;
    }

    StreamPointer OutputFormatContext::createStream(ParametersPointer params) {
        auto avstream = avformat_new_stream(mediaFormatContext(), params->codec());
        params->setStreamIndex(avstream->index);
        return std::make_shared<Stream>(avstream, params);
    }

    Code OutputFormatContext::guessOutputFromat() {// см: _format_context->oformat
        AVOutputFormat* output_format = av_guess_format(nullptr, mediaResourceLocator().c_str(), nullptr);
        if (output_format == nullptr) {
            log_error("Failed guess output format: " << mediaResourceLocator());
            return Code::INVALID_INPUT;
        }
        _output_format = output_format;
        return Code::OK;
    }

    Code OutputFormatContext::parseOutputFormat() {
        return Code::NOT_IMPLEMENTED;
//        return_if_not(inited_ptr(_output_format), Code::NOT_INITED);
//        {
//            //TODO:                                   ↓ mp3 AVOutputFormat дает видеокодек PNG ↓
//            auto& video_codec_id = _output_format->video_codec;
//            if (inited_codec_id(video_codec_id) && _output_format->video_codec != AV_CODEC_ID_PNG) {
//                try_to(createStream(utils::default_video_parameters(video_codec_id)));
//            }
//        }
//        {
//            auto& audio_codec_id = _output_format->audio_codec;
//            if (inited_codec_id(audio_codec_id)) {
//                try_to(createStream(utils::default_audio_parameters(audio_codec_id)));
//            }
//        }
//        return Code::OK;
    }

} // namespace fpp
