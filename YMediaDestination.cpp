#include "YMediaDestination.h"
#include <iostream>
#include <exception>

YMediaDestination::YMediaDestination(const std::string &mrl, YMediaPreset preset) :
	YAbstractMedia(mrl),
    _frame_index(0),
    _video_required(false),
    _audio_required(false),
    _output_format(nullptr)
{
    switch (preset) {
    case Auto:
         guessOutputFromat();
         parseOutputFormat();
        break;
    case Silence:
        break;
    case YouTube:
        _video_required = true;
        _audio_required = true;
        setWidth(1920);
        setHeight(1080);
        setAspectRatio({16,9});
        setFrameRate(30);
        setVideoCodecName("libx264");
        setAudioCodecName("aac");
        break;
    case Timelapse:
        break;
    }

    //
    if (!createOutputContext()) {
        std::cerr << "[YMediaDestination] Failed to create output context." << std::endl;
        return;
    }
}

YMediaDestination::~YMediaDestination()
{
    close(); //TODO падает на строке: _is_opened = false;
}

bool YMediaDestination::addStream(AVCodecContext *stream_codec_context)
{
    std::cout << "[YMediaDestination] Adding new " << stream_codec_context->codec->name << " stream" << std::endl;
    AVStream* out_stream = avformat_new_stream(_media_format_context, stream_codec_context->codec);
    if (out_stream == nullptr) {
        std::cerr << "[YMediaDestination] Failed allocating output stream" << std::endl;
        return false;
    }

    //
    out_stream->codecpar->width = 1920;
    out_stream->codecpar->height = 1080;
    //
    out_stream->codec->coded_width = 1920;
    out_stream->codec->coded_height = 1920;
    //

    if (avcodec_parameters_from_context(out_stream->codecpar, stream_codec_context) < 0) {
        std::cerr << "[YMediaDestination] Failed to copy context input to output stream codec context" << std::endl;
        return false;
    }

    auto codec_type = out_stream->codecpar->codec_type;

    switch (codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        _video_stream_index = _media_format_context->nb_streams - 1;
        break;
    case AVMEDIA_TYPE_AUDIO:
        _audio_stream_index = _media_format_context->nb_streams - 1;
        break;
    default:
        std::cerr << "[YMediaDestination] Unsupported media type added: " << av_get_media_type_string(codec_type) << std::endl;
        break;
    }

    return true;
}

bool YMediaDestination::open()
{
    std::cout << "[YMediaDestination] Opening" << std::endl;

//    if (!createOutputContext()) {
//        std::cerr << "[YMediaDestination] Failed to create output context." << std::endl;
//        return false;
//    }
//    if (!copyInputContext(_media_format_context, _video_required, _audio_required)) {
//        std::cerr << "Failed to copy input context." << std::endl;
//        return false;
//    }
    if (!openOutputContext()) {
        std::cerr << "[YMediaDestination] Failed to open output context." << std::endl;
        return false;
    }
    parseFormatContext();
    return true;
}

bool YMediaDestination::close()
{
    if (!_is_opened) { return false; }
    av_write_trailer(_media_format_context);

    YAbstractMedia::close();

    return true;
}

bool YMediaDestination::writePacket(AVPacket packet)
{
	if (!_is_opened) { return false; }

    if (packet.stream_index == 2) { return true; } //TODO

	switch (packet.stream_index) {
    case AVMEDIA_TYPE_VIDEO:
		stampPacket(packet);
		_frame_index++;
		break;
	case AVMEDIA_TYPE_AUDIO:
		stampPacket(packet);
		break;
	}

    auto size = packet.size;
//    if (av_write_frame(_media_format_context, &packet) < 0) {
    if (av_interleaved_write_frame(_media_format_context, &packet) < 0) {
        std::cerr << "[YMediaDestination] Error muxing packet" << std::endl;
		return false;
    } else {
        std::cerr << "[YMediaDestination] Writed " << size << std::endl;
    }

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

bool YMediaDestination::createOutputContext()
{
    std::string output_format_name = guessFormatShortName();
    const char *format_name = output_format_name.empty() ? nullptr : output_format_name.c_str();
    if (avformat_alloc_output_context2(&_media_format_context, nullptr, format_name, _media_resource_locator.c_str()) < 0) {
        std::cerr << "[YMediaDestination] Failed to alloc output context." << std::endl;
		return false;
    }
	return true;
}

bool YMediaDestination::openOutputContext()
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
		_is_opened = true;
		av_dump_format(_media_format_context, 0, _media_resource_locator.c_str(), 1);
        std::cout << "[YMediaDestination] Destination opened: \"" << _media_resource_locator << "\"" << std::endl;
	}
    return true;
}

void YMediaDestination::parseOutputFormat()
{
    if (_output_format == nullptr) { return; }
    setVideoCodecName(avcodec_get_name(_output_format->video_codec));
    setAudioCodecName(avcodec_get_name(_output_format->audio_codec));
    setVideoCodecId(_output_format->video_codec);
    setAudioCodecId(_output_format->audio_codec);
}

void YMediaDestination::stampPacket(AVPacket &packet)
{
    int64_t frame_duration = static_cast<int64_t>(1000.f / _frame_rate);
    packet.pts = _frame_index * frame_duration;
    packet.dts = packet.pts;
    packet.duration = (packet.stream_index = AVMEDIA_TYPE_VIDEO) ? frame_duration : packet.duration;
    packet.pos = -1;
}

void YMediaDestination::setVideoCodecContextOptions(AVCodecContext *avCodecContext)
{
	av_opt_set(avCodecContext->priv_data, "profile",	"high",			0);		// High Profile
	av_opt_set(avCodecContext->priv_data, "bf",			"2",			0);		// 2 consecutive B frames
	av_opt_set(avCodecContext->priv_data, "g",			"30",			0);		// Closed GOP. GOP of half the frame rate.
	av_opt_set(avCodecContext->priv_data, "codec",		"1",			0);		// CABAC
	av_opt_set(avCodecContext->priv_data, "crf",		"18",			0);		// Variable bitrate.
	av_opt_set(avCodecContext->priv_data, "pix_fmt",	"yuv420p",		0);		// Chroma subsampling: 4:2:0
	av_opt_set(avCodecContext->priv_data, "preset",		"veryfast",		0);

	av_opt_set(avCodecContext->priv_data, "flags",		"+cgop",		0);		// closed GOP as per guideline
	av_opt_set(avCodecContext->priv_data, "movflags",	"+faststart",	0);		// places moov atom/box at front of the output file.
}

void YMediaDestination::setAudioCodecContextOptions(AVCodecContext *avCodecContext)
{
	av_opt_set(avCodecContext->priv_data, "profile",	"aac_low",		0);		// AAC-LC
	av_opt_set(avCodecContext->priv_data, "c",			"aac",			0);		// AAC-LC
	av_opt_set(avCodecContext->priv_data, "b",			"384k",			0);		// Recommended audio bitrates for uploads: Stereo 384 kbps

	av_opt_set(avCodecContext->priv_data, "strict",		"-2",			0); 
}
