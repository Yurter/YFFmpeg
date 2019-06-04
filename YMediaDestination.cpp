#include "YMediaDestination.h"
#include <iostream>
#include <exception>

YMediaDestination::YMediaDestination(const std::string &mrl, YMediaPreset preset) :
	YAbstractMedia(mrl),
    _video_packet_index(0),
    _audio_packet_index(0),
    _output_format(nullptr)
{
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
//        video_parameters.setCodec("libx264");     //TODO rescaler
        video_parameters.setAvailable(true);
        /* Audio */
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
        std::cerr << "[YMediaDestination] Invalid preset." << std::endl;
        break;
    }

    //
    if (!createOutput()) {
        std::cerr << "[YMediaDestination] Failed to create output context." << std::endl;
        return;
    }
}

YMediaDestination::~YMediaDestination()
{
    close();
}

bool YMediaDestination::addStream(AVCodecContext *stream_codec_context)
{
    AVStream* out_stream = avformat_new_stream(_media_format_context, stream_codec_context->codec);
    if (out_stream == nullptr) {
        std::cerr << "[YMediaDestination] Failed allocating output stream" << std::endl;
        return false;
    }

    if (avcodec_parameters_from_context(out_stream->codecpar, stream_codec_context) < 0) {
        std::cerr << "[YMediaDestination] Failed to copy context input to output stream codec context" << std::endl;
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
        std::cerr << "[YMediaDestination] Unsupported media type added: " << av_get_media_type_string(codec_type) << std::endl;
        break;
    }

    std::cout << "[YMediaDestination] Created stream: " << stream_codec_context->codec->name << std::endl;
    return true;
}

bool YMediaDestination::open()
{
    if (_opened) { return false; }
    _opened = openOutput();
    if (_opened) {
        parseFormatContext();
//        start();
    }
    return _opened;
}

bool YMediaDestination::close()
{
    if (!_opened) { return false; }
    quit();
    if (av_write_trailer(_media_format_context) != 0) {
        std::cerr << "[YMediaDestination] Failed to write the stream trailer to an output media file" << std::endl;
        return false;
    }
    if (!YAbstractMedia::close()) { return false; }
    std::cout << "[YMediaDestination] Destination: \"" << _media_resource_locator << "\" closed." << std::endl;
    return true;
}

AVOutputFormat *YMediaDestination::outputFrormat() const
{
    return _output_format;
}

bool YMediaDestination::guessOutputFromat()
{
    AVOutputFormat* output_format = av_guess_format(nullptr, _media_resource_locator.c_str(), nullptr);
    if (output_format == nullptr) {
        std::cerr << "[YAbstractMedia] Failed guess output format: " << _media_resource_locator << std::endl;
        return false;
    }
    _output_format = output_format;
    return true;
}

bool YMediaDestination::createOutput()
{
    std::string output_format_name = guessFormatShortName();
    const char *format_name = output_format_name.empty() ? nullptr : output_format_name.c_str();
    if (avformat_alloc_output_context2(&_media_format_context, nullptr, format_name, _media_resource_locator.c_str()) < 0) {
        std::cerr << "[YMediaDestination] Failed to alloc output context." << std::endl;
		return false;
    }
    _output_format = _media_format_context->oformat;
	return true;
}

bool YMediaDestination::openOutput()
{
	if (!(_media_format_context->flags & AVFMT_NOFILE)) {
		if (avio_open(&_media_format_context->pb, _media_resource_locator.c_str(), AVIO_FLAG_WRITE) < 0) {
            std::cerr << "[YMediaDestination] Could not open output: " << _media_resource_locator << std::endl;
			return false;
		}
	}
	if (avformat_write_header(_media_format_context, nullptr) < 0) {
        std::cerr << "[YMediaDestination] Error occurred when opening output" << std::endl;
		return false;
	}
	{
		av_dump_format(_media_format_context, 0, _media_resource_locator.c_str(), 1);
        std::cout << "[YMediaDestination] Destination: \"" << _media_resource_locator << "\" opened." << std::endl;
	}
    return true;
}

void YMediaDestination::parseOutputFormat()
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

YCode YMediaDestination::run()
{
    YPacket packet;
    if (!_packet_queue.pop(packet)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return YCode::AGAIN;
    }
    if (!stampPacket(packet)) {
        std::cerr << "[YMediaDestination] stampPacket failed" << std::endl;
        return YCode::ERR;
    }
    auto debug_dts = packet.raw().dts;
    auto debug_pkt = packet;
    auto debug_sti = packet.raw().stream_index;
    auto debug_typ = packet.type();
    if (debug_sti == 0 && debug_typ == MEDIA_TYPE_AUDIO) {
        int stop = 0;
        packet.raw().stream_index = 1;
    }
    {
        std::cout << "[YMediaDestination] " << packet.toString() << std::endl;
    }
    if (av_interleaved_write_frame(_media_format_context, &packet.raw()) < 0) {
//    if (av_write_frame(_media_format_context, &packet.raw()) < 0) {
        std::cerr << "[YMediaDestination] this_thread " << std::this_thread::get_id()
                  << " stream_index: " << debug_sti
                  << " type: " << utils::media_type_to_string(debug_typ)
                  << std::endl;
        std::cerr << "[YMediaDestination] Error muxing packet " << " dts: " << debug_dts/*packet.toString()*/ << " " << (debug_dts == AV_NOPTS_VALUE) << std::endl;
        return YCode::ERR;
    }
    return YCode::OK;
}

bool YMediaDestination::stampPacket(YPacket &packet)
{
    if (packet.isVideo()) {
        auto&& raw_packet = packet.raw();
//        auto frame_rate = stream(packet.raw()->stream_index)->avg_frame_rate; //TODO : проверить (!)
//        auto duration = (1000 * frame_rate.den) / frame_rate.num;
        auto frame_rate = video_parameters.frameRate();
        int64_t duration = static_cast<int64_t>(1000 / frame_rate);
        raw_packet.pts = _video_packet_index * duration;
        raw_packet.dts = _video_packet_index * duration;
        raw_packet.duration = duration;
        raw_packet.pos = -1;
        _video_packet_index++;
        return true;
    }
    if (packet.isAudio()) {
        _audio_packet_index++;
        return true;
    }
    return false;
}
