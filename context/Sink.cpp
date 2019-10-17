#include "Sink.hpp"
#include <exception>

namespace fpp {

    Sink::Sink(const std::string& mrl, MediaPreset preset) :
        Context(mrl, preset),
        _output_format(nullptr)
    {
        setName("Sink");
    }

    Sink::~Sink() {
        close();
    }

    Code Sink::init() {
        try_to(createContext());
        switch (_preset) {
        case Auto: {
            try_to(guessOutputFromat());
//            try_to(parseOutputFormat());  // В автоматическом режиме Синк не должен самостоятельно создаввать потоки (устанавливать требования),
                                            // только повторять входные.
            break;
        }
        case YouTube: {
            /* Video */
            auto video_parameters = new VideoParameters;
            video_parameters->setWidth(1920);
            video_parameters->setHeight(1080);
            video_parameters->setAspectRatio({16,9});
            video_parameters->setFrameRate(24); //TODO
            video_parameters->setBitrate(400'000);
            video_parameters->setCodec("libx264", CodecType::Encoder);
            video_parameters->setTimeBase({ 1, 1000 });
            video_parameters->setContextUid(uid());
            try_to(createStream(video_parameters));
            /* Audio */
            auto audio_parameters = new AudioParameters;
            audio_parameters->setSampleRate(44'100);
    //        audio_parameters->setSampleFormat(AV_SAMPLE_FMT_FLTP);
            audio_parameters->setSampleFormat(AV_SAMPLE_FMT_FLTP);
            audio_parameters->setBitrate(128'000);
            audio_parameters->setChannelLayout(AV_CH_LAYOUT_STEREO);
            audio_parameters->setChannels(2);
            audio_parameters->setCodec("aac", CodecType::Encoder);
    //        audio_parameters->setChannelLayout(AV_CH_LAYOUT_MONO);
    //        audio_parameters->setChannels(1);
    //        audio_parameters->setCodec("aac");
            audio_parameters->setContextUid(uid());
            try_to(createStream(audio_parameters));
            break;
        }
        case Timelapse: {
            /* Video */
            auto video_parameters = new VideoParameters;
//            video_parameters->setWidth(1920);
//            video_parameters->setHeight(1080);
            video_parameters->setWidth(300);
            video_parameters->setHeight(300);
//            video_parameters->setAspectRatio({ 16, 9 });
            video_parameters->setAspectRatio({ 1, 1 });
            video_parameters->setFrameRate(24); //TODO
            video_parameters->setBitrate(400'000);
            video_parameters->setCodec("libx264", CodecType::Encoder);
            video_parameters->setTimeBase({ 1, 1000 });
            video_parameters->setPixelFormat(AV_PIX_FMT_YUV420P);
            video_parameters->setContextUid(uid());
            try_to(createStream(video_parameters));
            break;
        }
        default:
            log_error("Invalid preset");
            break;
        }
//        if (numberStream() == 0) {
//            log_error("No streams to mux were specified");
//            return Code::NOT_INITED;
//        }
        log_debug("Created");
        setInited(true);
        return Code::OK;
    }

    Code Sink::open() {
        return_if(opened(), Code::INVALID_CALL_ORDER);
        return_if_not(inited(), Code::NOT_INITED);
        _output_format = _format_context->oformat; //TODO оператор "болтается в воздухе"
        try_to(openContext());
        _io_thread = Thread(std::bind(&Sink::write, this));
        _io_thread.setName("IOThread");
        _io_thread.start();
        setInited(true);
        return Code::OK;
    }

    Code Sink::close() {
//        log_debug("closing " << mediaResourceLocator() << " " << closed());
        return_if(closed(), Code::INVALID_CALL_ORDER);
        quit();
        if (av_write_trailer(_format_context) != 0) {
            log_error("Failed to write the stream trailer to an output media file");
            return Code::ERR;
        }
        try_to(Context::close());
        log_info("Destination: \"" << _media_resource_locator << "\" closed.");
        return Code::OK;
    }

    std::string Sink::toString() const {
        /* Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/ytub-8t5w-asjj-avyf'*/
        std::string str = "Output #"
                + std::to_string(uid()) + ", "
                + utils::guess_format_short_name(_media_resource_locator) + ", "
                + "to '" + _media_resource_locator + "'";
        return str;
    }

    AVOutputFormat* Sink::outputFrormat() const {
        return _output_format;
    }

    Code Sink::guessOutputFromat() {// см: _format_context->oformat
        AVOutputFormat* output_format = av_guess_format(nullptr, _media_resource_locator.c_str(), nullptr);
        if (output_format == nullptr) {
            log_error("Failed guess output format: " << _media_resource_locator);
            return Code::INVALID_INPUT;
        }
        _output_format = output_format;
        return Code::OK;
    }

    Code Sink::createContext() {
        std::string format_short_name = utils::guess_format_short_name(_media_resource_locator);
        const char* format_name = format_short_name.empty() ? nullptr : format_short_name.c_str();
        if (avformat_alloc_output_context2(&_format_context, nullptr, format_name, _media_resource_locator.c_str()) < 0) {
            log_error("Failed to alloc output context.");
            return Code::ERR;
        }
        return Code::OK;
    }

    Code Sink::openContext() {
        log_info("Destination: \"" << _media_resource_locator << "\" is opening...");
        if (_streams.empty()) {
            log_error("No streams to mux were specified");
            return Code::NOT_INITED;
        }
        if (!(_format_context->flags & AVFMT_NOFILE)) {
            if (avio_open(&_format_context->pb, _media_resource_locator.c_str(), AVIO_FLAG_WRITE) < 0) {
                log_error("Could not open output: " << _media_resource_locator);
                return Code::INVALID_INPUT;
            }
        }
        //AVSTREAM_INIT_IN_WRITE_HEADER
        //AVSTREAM_INIT_IN_INIT_OUTPUT
    //    avformat_write_header(_format_context, nullptr); //TODO check return value?
        if (avformat_write_header(_format_context, nullptr) < 0) {
            log_error("Error occurred when opening output: " << _media_resource_locator);
            return Code::ERR;
        }
        {
            av_dump_format(_format_context, 0, _media_resource_locator.c_str(), 1);
            log_info("Destination: \"" << _media_resource_locator << "\" opened.");
            setOpened(true);
            return Code::OK;
        }
    }

    Code Sink::write() {
        return Code::OK;
    }

    Code Sink::writePacket(Packet& packet) {
        log_debug(packet);
        if (av_interleaved_write_frame(_format_context, &packet.raw()) < 0) {
    //    if (av_write_frame(_format_context, &packet.raw()) < 0) {
            log_error("Error muxing packet");
            return Code::ERR;
        }
        return Code::OK;
    }

    Code Sink::processInputData(Packet& input_data) {
        log_debug("Got packet: " << input_data);
        if (input_data.isVideo()) { //Debug if
            try_to(stream(input_data.streamIndex())->stampPacket(input_data));
        }
        try_to(writePacket(input_data));
        return Code::OK;
    }

    Code Sink::parseOutputFormat() {
        return_if_not(inited_ptr(_output_format), Code::INVALID_CALL_ORDER);
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
