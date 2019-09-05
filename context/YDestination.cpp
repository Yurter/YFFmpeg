#include "YDestination.hpp"
#include <exception>

YDestination::YDestination(const std::string& mrl, YMediaPreset preset) :
    YContext(mrl, preset),
    _output_format(nullptr) {
    setName("YDestination");
}

YDestination::~YDestination() {
    close();
}

YCode YDestination::init() {
    try_to(createContext());
    switch (_preset) {
    case Auto: {
        try_to(guessOutputFromat());
        try_to(parseOutputFormat());
        break;
    }
    case YouTube: {
        /* Video */
        auto video_parameters = new YVideoParameters;
        video_parameters->setWidth(1920);
        video_parameters->setHeight(1080);
        video_parameters->setAspectRatio({16,9});
        video_parameters->setFrameRate(24); //TODO
        video_parameters->setBitrate(400'000);
        video_parameters->setCodec("libx264");
        video_parameters->setTimeBase({ 1, 1000 });
        video_parameters->setContextUid(uid());
        try_to(createStream(video_parameters));
        /* Audio */
        auto audio_parameters = new YAudioParameters;
        audio_parameters->setSampleRate(44'100);
//        audio_parameters->setSampleFormat(AV_SAMPLE_FMT_FLTP);
        audio_parameters->setSampleFormat(AV_SAMPLE_FMT_FLTP);
        audio_parameters->setBitrate(128'000);
        audio_parameters->setChannelLayout(AV_CH_LAYOUT_STEREO);
        audio_parameters->setChannels(2);
        audio_parameters->setCodec("aac");
//        audio_parameters->setChannelLayout(AV_CH_LAYOUT_MONO);
//        audio_parameters->setChannels(1);
//        audio_parameters->setCodec("aac");
        audio_parameters->setContextUid(uid());
        try_to(createStream(audio_parameters));
        break;
    }
    case Timelapse:
        break;
    default:
        log_error("Invalid preset");
        break;
    }
    if (numberStream() == 0) {
        log_error("No streams to mux were specified");
        return YCode::NOT_INITED;
    }
    setInited(true);
    return YCode::OK;
}

YCode YDestination::open() {
    return_if(opened(), YCode::INVALID_CALL_ORDER);
    return_if_not(inited(), YCode::NOT_INITED);
    _output_format = _format_context->oformat; //TODO оператор "болтается в воздухе"
    try_to(openContext());
    _io_thread = YThread(std::bind(&YDestination::write, this));
    _io_thread.setName("IOThread");
    _io_thread.start();
    setInited(true);
    return YCode::OK;
}

YCode YDestination::close()
{
    return_if(closed(), YCode::INVALID_CALL_ORDER);
    quit();
    if (av_write_trailer(_format_context) != 0) {
        log_error("Failed to write the stream trailer to an output media file");
        return YCode::ERR;
    }
    try_to(YContext::close());
    log_info("Destination: \"" << _media_resource_locator << "\" closed.");
    return YCode::OK;
}

std::string YDestination::toString() const {
    /* Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/ytub-8t5w-asjj-avyf'*/
    std::string str = "Output #"
            + std::to_string(uid()) + ", "
            + utils::guess_format_short_name(_media_resource_locator) + ", "
            + "to '" + _media_resource_locator + "'";
    return str;
}

AVOutputFormat* YDestination::outputFrormat() const {
    return _output_format;
}

YCode YDestination::guessOutputFromat() {
    AVOutputFormat* output_format = av_guess_format(nullptr, _media_resource_locator.c_str(), nullptr);
    if (output_format == nullptr) {
        log_error("Failed guess output format: " << _media_resource_locator);
        return YCode::INVALID_INPUT;
    }
    _output_format = output_format;
    return YCode::OK;
}

YCode YDestination::createContext() {
    std::string format_short_name = utils::guess_format_short_name(_media_resource_locator);
    const char* format_name = format_short_name.empty() ? nullptr : format_short_name.c_str();
    if (avformat_alloc_output_context2(&_format_context, nullptr, format_name, _media_resource_locator.c_str()) < 0) {
        log_error("Failed to alloc output context.");
        return YCode::ERR;
    }
    return YCode::OK;
}

YCode YDestination::openContext() {
    log_info("Destination: \"" << _media_resource_locator << "\" is opening...");
    if (!(_format_context->flags & AVFMT_NOFILE)) {
        if (avio_open(&_format_context->pb, _media_resource_locator.c_str(), AVIO_FLAG_WRITE) < 0) {
            log_error("Could not open output: " << _media_resource_locator);
            return YCode::INVALID_INPUT;
        }
    }
    //AVSTREAM_INIT_IN_WRITE_HEADER
    //AVSTREAM_INIT_IN_INIT_OUTPUT
//    avformat_write_header(_format_context, nullptr); //TODO check return value?
    if (avformat_write_header(_format_context, nullptr) < 0) {
        log_error("Error occurred when opening output");
        return YCode::ERR;
    }
    {
        av_dump_format(_format_context, 0, _media_resource_locator.c_str(), 1);
        log_info("Destination: \"" << _media_resource_locator << "\" opened.");
        return YCode::OK;
    }
}

YCode YDestination::write() {
    return YCode::OK;
}

YCode YDestination::writePacket(YPacket& packet) {
//    log_debug(packet);
    if (av_interleaved_write_frame(_format_context, &packet.raw()) < 0) {
//    if (av_write_frame(_format_context, &packet.raw()) < 0) {
        log_error("Error muxing packet");
        return YCode::ERR;
    }
    return YCode::OK;
}

YCode YDestination::processInputData(YPacket& input_data) {
    if (input_data.isVideo()) { //Debug if
        try_to(stream(input_data.streamIndex())->stampPacket(input_data));
    }
    try_to(writePacket(input_data));
    return YCode::OK;
}

YCode YDestination::parseOutputFormat() {
    return_if_not(inited_ptr(_output_format), YCode::INVALID_CALL_ORDER);
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
    return YCode::OK;
}
