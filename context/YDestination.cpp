#include "YDestination.h"
#include <exception>

YDestination::YDestination(const std::string &mrl, YMediaPreset preset) :
	YAbstractMedia(mrl),
    _video_packet_index(0),
    _audio_packet_index(0),
    _output_format(nullptr)
{
    setName("YDestination");
    switch (preset) {
    case Auto:
         guessOutputFromat();
         parseOutputFormat();
        break;
    case YouTube:
        /* Video */
//        video_parameters.setWidth(1920);          //TODO rescaler
//        video_parameters.setHeight(1080);         //TODO rescaler
        video_parameters.setAspectRatio({16,9});
//        video_parameters.setFrameRate(24);        //TODO rescaler
        video_parameters.setBitrate(400'000);
//        video_parameters.setCodec("libx264");     //TODO rescaler ?
        video_parameters.setAvailable(true);
        /* Audio */
        /* mp3 */
//        audio_parameters.setSampleRate(44'100);
//        audio_parameters.setSampleFormat(AV_SAMPLE_FMT_S16P);
//        audio_parameters.setBitrate(128'000);
//        audio_parameters.setChanelsLayout(AV_CH_LAYOUT_STEREO);
//        audio_parameters.setChanels(2);
//        audio_parameters.setCodec("mp3");
//        audio_parameters.setAvailable(true);
        /* aac */
        audio_parameters.setSampleRate(44'100);
        audio_parameters.setSampleFormat(AV_SAMPLE_FMT_FLTP);
        audio_parameters.setBitrate(128'000);
        audio_parameters.setChanelsLayout(AV_CH_LAYOUT_STEREO);
        audio_parameters.setChanels(2);
        audio_parameters.setCodec("aac");
        audio_parameters.setAvailable(true);
        break;
    case Timelapse:
        break;
    default:
        log_error("Invalid preset");
        break;
    }
//    if (!createOutput()) {
//        log_error("Failed to create output context");
//        return;
//    }
}

YDestination::~YDestination()
{
    close();
}

//bool YDestination::addStream(AVCodecContext *stream_codec_context)
//{
//    AVStream* out_stream = avformat_new_stream(_media_format_context, stream_codec_context->codec);
//    if (out_stream == nullptr) {
//        log_error("Failed allocating output stream");
//        return false;
//    }

//    if (avcodec_parameters_from_context(out_stream->codecpar, stream_codec_context) < 0) {
//        log_error("Failed to copy context input to output stream codec context");
//        return false;
//    }

//    /* Crutch */
//    out_stream->codec->sample_fmt = stream_codec_context->sample_fmt;

//    out_stream->r_frame_rate = stream_codec_context->framerate;
//    out_stream->avg_frame_rate = stream_codec_context->framerate;

//    auto codec_type = out_stream->codecpar->codec_type;

//    switch (codec_type) {
//    case AVMEDIA_TYPE_VIDEO:
//        video_parameters.setStreamIndex(_media_format_context->nb_streams - 1);
//        break;
//    case AVMEDIA_TYPE_AUDIO:
//        audio_parameters.setStreamIndex(_media_format_context->nb_streams - 1);
//        break;
//    default:
//        log_error("Unsupported media type added: " << av_get_media_type_string(codec_type));
//        break;
//    }

//    log_info("Created stream: " << stream_codec_context->codec->name);
//    return true;
//}

YCode YDestination::open()
{
    return_if(opened(), YCode::INVALID_CALL_ORDER);
    try_to(createContext());
    _output_format = _media_format_context->oformat; //TODO оператор "болтается в воздухе"
    try_to(openContext());
    try_to(parseFormatContext());
    _io_thread = YThread(std::bind(&YDestination::write, this));
    _io_thread.start();
    return YCode::OK;
}

YCode YDestination::close()
{
    return_if(closed(), YCode::INVALID_CALL_ORDER);
    quit();
    if (av_write_trailer(_media_format_context) != 0) {
        log_error("Failed to write the stream trailer to an output media file");
        return YCode::ERR;
    }
    try_to(YAbstractMedia::close());
    log_info("Destination: \"" << _media_resource_locator << "\" closed.");
    return YCode::OK;
}

AVOutputFormat *YDestination::outputFrormat() const
{
    return _output_format;
}

YCode YDestination::guessOutputFromat()
{
    AVOutputFormat* output_format = av_guess_format(nullptr, _media_resource_locator.c_str(), nullptr);
    if (output_format == nullptr) {
        log_error("Failed guess output format: " << _media_resource_locator);
        return YCode::INVALID_INPUT;
    }
    _output_format = output_format;
    return YCode::OK;
}

YCode YDestination::openContext()
{
    if (!(_media_format_context->flags & AVFMT_NOFILE)) {
        if (avio_open(&_media_format_context->pb, _media_resource_locator.c_str(), AVIO_FLAG_WRITE) < 0) {
            log_error("Could not open output: " << _media_resource_locator);
            return YCode::INVALID_INPUT;
        }
    }
    if (avformat_write_header(_media_format_context, nullptr) < 0) {
        log_error("Error occurred when opening output");
        return YCode::ERR;
    }
    {
        av_dump_format(_media_format_context, 0, _media_resource_locator.c_str(), 1);
        log_info("Destination: \"" << _media_resource_locator << "\" opened.");
        return YCode::OK;
    }
}

YCode YDestination::write()
{
    return YCode::OK;
}

YCode YDestination::stampPacket(YPacket& packet) //TODO кривой вызов функции
{
    try_to(packet.stream()->stampPacket(packet)); //TODO кривой вызов функции
}

YCode YDestination::writePacket(YPacket& packet)
{
    if (av_interleaved_write_frame(_media_format_context, &packet.raw()) < 0) {
//    if (av_write_frame(_media_format_context, &input_data.raw()) < 0) {
        log_error("Error muxing packet");
        return YCode::ERR;
    }
    return YCode::OK;
}

YCode YDestination::processInputData(YPacket& input_data)
{
    try_to(stampPacket(input_data));
    try_to(writePacket(input_data));
    return YCode::OK;
}

void YDestination::parseOutputFormat()
{
    if (_output_format == nullptr) { return; }
    //TODO: mp3 AVOutputFormat дает видео кодек PNG
    if (_output_format->video_codec != AV_CODEC_ID_NONE && _output_format->video_codec != AV_CODEC_ID_PNG) {
        video_parameters.setCodec(_output_format->video_codec);
        video_parameters.setAvailable(true);
    }
    if (_output_format->audio_codec != AV_CODEC_ID_NONE) {
        audio_parameters.setCodec(_output_format->audio_codec);
        audio_parameters.setAvailable(true);
    }
}