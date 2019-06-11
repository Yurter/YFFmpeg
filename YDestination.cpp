#include "YDestination.h"
#include <iostream>
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
    if (!createOutput()) {
        log_error("Failed to create output context");
        return;
    }
}

YDestination::~YDestination()
{
    close();
}

bool YDestination::addStream(AVCodecContext *stream_codec_context)
{
    AVStream* out_stream = avformat_new_stream(_media_format_context, stream_codec_context->codec);
    if (out_stream == nullptr) {
        log_error("Failed allocating output stream");
        return false;
    }

    if (avcodec_parameters_from_context(out_stream->codecpar, stream_codec_context) < 0) {
        log_error("Failed to copy context input to output stream codec context");
        return false;
    }

    /* Crutch */
    out_stream->codec->sample_fmt = stream_codec_context->sample_fmt;

    out_stream->r_frame_rate = stream_codec_context->framerate;
    out_stream->avg_frame_rate = stream_codec_context->framerate;

    auto codec_type = out_stream->codecpar->codec_type;

    switch (codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        video_parameters.setStreamIndex(_media_format_context->nb_streams - 1);
        break;
    case AVMEDIA_TYPE_AUDIO:
        audio_parameters.setStreamIndex(_media_format_context->nb_streams - 1);
        break;
    default:
        log_error("Unsupported media type added: " << av_get_media_type_string(codec_type));
        break;
    }

    log_info("Created stream: " << stream_codec_context->codec->name);
    return true;
}

bool YDestination::open()
{
    if (_opened) { return false; }
    _opened = openOutput();
    if (_opened) {
        parseFormatContext();
        _io_thread = YThread(std::bind(&YDestination::write, this));
        _io_thread.start();
    }
    return _opened;
}

bool YDestination::close()
{
    if (!_opened) { return false; }
    quit();
    if (av_write_trailer(_media_format_context) != 0) {
        log_error("Failed to write the stream trailer to an output media file");
        return false;
    }
    if (!YAbstractMedia::close()) { return false; }
    log_info("Destination: \"" << _media_resource_locator << "\" closed.");
    return true;
}

AVOutputFormat *YDestination::outputFrormat() const
{
    return _output_format;
}

bool YDestination::guessOutputFromat()
{
    AVOutputFormat* output_format = av_guess_format(nullptr, _media_resource_locator.c_str(), nullptr);
    if (output_format == nullptr) {
        std::cerr << "[YAbstractMedia] Failed guess output format: " << _media_resource_locator << std::endl;
        return false;
    }
    _output_format = output_format;
    return true;
}

bool YDestination::createOutput()
{
    std::string output_format_name = guessFormatShortName();
    const char *format_name = output_format_name.empty() ? nullptr : output_format_name.c_str();
    if (avformat_alloc_output_context2(&_media_format_context, nullptr, format_name, _media_resource_locator.c_str()) < 0) {
        log_error("Failed to alloc output context.");
		return false;
    }
    _output_format = _media_format_context->oformat;
	return true;
}

bool YDestination::openOutput()
{
	if (!(_media_format_context->flags & AVFMT_NOFILE)) {
		if (avio_open(&_media_format_context->pb, _media_resource_locator.c_str(), AVIO_FLAG_WRITE) < 0) {
            log_error("Could not open output: " << _media_resource_locator);
			return false;
		}
	}
	if (avformat_write_header(_media_format_context, nullptr) < 0) {
        log_error("Error occurred when opening output");
		return false;
	}
	{
		av_dump_format(_media_format_context, 0, _media_resource_locator.c_str(), 1);
        log_info("Destination: \"" << _media_resource_locator << "\" opened.");
	}
    return true;
}

YCode YDestination::write()
{
//    YPacket packet;
//    if (!pop(packet)) {
//        utils::sleep_for(SHORT_DELAY_MS);
//        return YCode::AGAIN;
//    }
//    print_func
//    { // debug
//        std::cout << "[YDestination] " << packet.toString() << std::endl;
//        std::cout << "[YDestination] v: " << stream(0)->duration() << ", a: " << stream(1)->duration() << std::endl << std::endl;
//        std::cout << "[YDestination] v: " << _video_packet_index << ", a: " << _audio_packet_index << std::endl << std::endl;
//    }
//    if (av_interleaved_write_frame(_media_format_context, &packet.raw()) < 0) {
//        std::cerr << "[YDestination] Error muxing packet" << std::endl;
//        return YCode::ERR;
//    }
    return YCode::OK;
}

YCode YDestination::processInputData(YPacket& input_data)
{
//    print_func
    if (input_data.isAudio()) {
        int debuf_stop = 0;
    }
    if (!stampPacket(input_data)) {
        log_error("stampPacket failed");
        return YCode::ERR;
    }
    //TODO это д.б. в датапроцессоре
    if (input_data.isVideo() && video_parameters.ignore()) { return YCode::AGAIN; }
    if (input_data.isAudio() && audio_parameters.ignore()) { return YCode::AGAIN; }
    { // debug
        log_debug(input_data.toString());
//        std::cout << "[YDestination] packets v: " << stream(0)->duration() << ", a: " << stream(1)->duration() << std::endl << std::endl;
//        std::cout << "[YDestination] v: " << _video_packet_index << ", a: " << _audio_packet_index << std::endl << std::endl;
    }
//    if (av_interleaved_write_frame(_media_format_context, &input_data.raw()) < 0) {
    if (av_write_frame(_media_format_context, &input_data.raw()) < 0) {
        log_error("Error muxing packet");
        return YCode::ERR;
    }
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

bool YDestination::stampPacket(YPacket &packet) //TODO перенести код в YStream
{
//    if (packet.isVideo()) {
//        auto&& raw_packet = packet.raw();
//        auto frame_rate = video_parameters.frameRate();
//        int64_t duration = static_cast<int64_t>(1000 / frame_rate);
//        raw_packet.pts = _video_packet_index * duration;
//        raw_packet.dts = _video_packet_index * duration;
//        raw_packet.duration = duration;
//        raw_packet.pos = -1;
//        _video_packet_index++;
//        return true;
//    }
//    if (packet.isAudio()) {
//        _audio_packet_index++;
//        return true;
//    }
    if (packet.isVideo()) {
        auto frame_rate = video_parameters.frameRate();
        int64_t duration = static_cast<int64_t>(1000 / frame_rate);
        auto video_stream = stream(static_cast<uint64_t>(packet.streamIndex()));
        video_stream->increaseDuration(duration);
        packet.setPts(video_stream->duration());
        packet.setDts(video_stream->duration());
        packet.setDuration(duration);
        packet.setPos(-1);
        _video_packet_index++;
        return true;
    }
    if (packet.isAudio()) {
////        int64_t duration = 1000 / 23;//44;//packet.duration();
//        int64_t duration = av_rescale_q(packet.duration(), {1,44100}, {1,1000});
////        std::cout << "[DEBUG] duration: " << duration << std::endl;
//        auto audio_stream = stream(static_cast<uint64_t>(packet.streamIndex()));
//        audio_stream->increaseDuration(duration);
//        packet.setPts(audio_stream->duration());
//        packet.setDts(audio_stream->duration());
//        packet.setDuration(duration);
        //
//        packet.setPts(av_rescale_q(packet.pts(), {1,44100}, {1,1000}));
//        packet.setDts(av_rescale_q(packet.dts(), {1,44100}, {1,1000}));
//        packet.setDuration(av_rescale_q(packet.duration(), {1,44100}, {1,1000}));
//        packet.setDuration(duration);
//        std::cout << "[DEBUG] " << packet.toString() << std::endl;
        //
//        auto video_stream = stream(static_cast<uint64_t>(0));
////        packet.setPts(video_stream->duration());
////        packet.setDts(video_stream->duration());
////        packet.setDuration(duration);
//        //
//        packet.setPos(-1);
        //
//        return true;
        int64_t duration = 8;//23;//43;//23;
        auto audio_stream = stream(static_cast<uint64_t>(packet.streamIndex()));
        audio_stream->increaseDuration(duration);
//        packet.setPts(audio_stream->duration());
//        packet.setDts(audio_stream->duration());
//        packet.setDuration(duration);
        packet.setPos(-1);
        audio_stream->increaseDuration(duration);
        _audio_packet_index++;
        return true;
    }
    return false;
}
