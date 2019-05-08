#include "YMediaDestination.h"
#include <iostream>
#include <exception>

YMediaDestination::YMediaDestination(const std::string &mrl, YMediaPreset preset) :
	YAbstractMedia(mrl),
    _frame_index(0),
    _video_required(false),
    _audio_required(false)
{
    switch (preset) {
    case Auto:
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
        setVideoCodecName("h264");//flv1//h264
        setAudioCodecName("aac");//pcm_mulaw//aac
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

    if (avcodec_parameters_from_context(out_stream->codecpar, stream_codec_context) < 0) {
        std::cerr << "[YMediaDestination] Failed to copy context input to output stream codec context" << std::endl;
        return false;
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
    getInfo();
    return true;
}

bool YMediaDestination::writePacket(AVPacket packet)
{
	if (!_is_opened) { return false; }

	if (_lavfi_video_format_context != nullptr) {
		if (!muxVideoPacket()) {
			return false;
		}
	}

	if (_lavfi_audio_format_context != nullptr) {
		if (!muxAudioPacket()) {
			return false;
		}
	}

	switch (packet.stream_index) {
	case AVMEDIA_TYPE_VIDEO:
		stampPacket(packet);
		_frame_index++;
		break;
	case AVMEDIA_TYPE_AUDIO:
		stampPacket(packet);
		break;
	}

	if (av_interleaved_write_frame(_media_format_context, &packet) < 0) {
        std::cerr << "[YMediaDestination] Error muxing packet" << std::endl;
		return false;
	}

    return true;
}

bool YMediaDestination::close()
{
    if (!_is_opened) { return false; }
	av_write_trailer(_media_format_context);
	_is_opened = false;
    return true;
}

bool YMediaDestination::createOutputContext()
{
	std::string output_format_name = guessFormatName();
	const char *format_name = output_format_name.empty() ? nullptr : output_format_name.c_str();
	if (avformat_alloc_output_context2(&_media_format_context, nullptr, format_name, _media_resource_locator.c_str()) < 0) {
        std::cerr << "[YMediaDestination] Failed to alloc output context." << std::endl;
		return false;
    }
	return true;
}

bool YMediaDestination::copyInputContext(const AVFormatContext *input_format_context, bool audio_required, bool video_required)
{
	bool inputHasVideo = false;
	bool inputHasAudio = false;

	for (uint64_t i = 0; i < input_format_context->nb_streams; i++) {
		auto codec_type = input_format_context->streams[i]->codecpar->codec_type;

		if (codec_type == AVMEDIA_TYPE_VIDEO || codec_type == AVMEDIA_TYPE_AUDIO) {
			AVStream* in_stream = input_format_context->streams[i];
			AVCodec *decoder = avcodec_find_decoder(in_stream->codecpar->codec_id);
			if (decoder == nullptr) {
                std::cerr << "[YMediaDestination] Could not find decoder" << std::endl;
				return false;
			}
			AVStream* out_stream = avformat_new_stream(_media_format_context, decoder);
			if (out_stream == nullptr) {
                std::cerr << "[YMediaDestination] Failed allocating output stream" << std::endl;
				return false;
			}
			AVCodecContext* avCodecContext = avcodec_alloc_context3(decoder);
			if (avcodec_parameters_to_context(avCodecContext, in_stream->codecpar) < 0) {
				avcodec_free_context(&avCodecContext);
				avCodecContext = nullptr;
                std::cerr << "[YMediaDestination] Can not fill codec context" << std::endl;
				return false;
			}
			if (codec_type == AVMEDIA_TYPE_VIDEO) {
				inputHasVideo = true;
				setVideoCodecContextOptions(avCodecContext);
				out_stream->avg_frame_rate = in_stream->avg_frame_rate;
				out_stream->r_frame_rate = in_stream->r_frame_rate;
			}
			if (codec_type == AVMEDIA_TYPE_AUDIO) {
				inputHasAudio = true;
				setAudioCodecContextOptions(avCodecContext);
			}
			avCodecContext->codec_tag = 0;
			if (_media_format_context->oformat->flags & AVFMT_GLOBALHEADER) {
				avCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
			}
			if (avcodec_parameters_from_context(out_stream->codecpar, avCodecContext) < 0) {
                std::cerr << "[YMediaDestination] Failed to copy context input to output stream codec context" << std::endl;
				return false;
			}
		}
		else {
			continue;
		}
	}

	if (!inputHasVideo && video_required) {
		openVirtualVideoInput();
	}
	if (!inputHasAudio && audio_required) {
		openVirtualAudioInput();
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

void YMediaDestination::stampPacket(AVPacket &packet)
{
    int64_t frame_duration = static_cast<int64_t>(1000.f / _frame_rate);
    packet.pts = _frame_index * frame_duration;
    packet.dts = packet.pts;
    packet.duration = frame_duration;
    packet.pos = -1;
}

bool YMediaDestination::muxVideoPacket()
{
	return false;
}

bool YMediaDestination::muxAudioPacket()
{
	int frame_rate_int = static_cast<int>(_frame_rate);
	if ((_frame_index % frame_rate_int) == 0) {
		AVPacket audio_packet = readVirtualAudioPacket();
		if (av_interleaved_write_frame(_media_format_context, &audio_packet) < 0) {
            std::cerr << "[YMediaDestination] Error muxing virtual audio packet" << std::endl;
			return false;
		}
	}
    return true;
}

void YMediaDestination::openVirtualAudioInput()
{
    AVInputFormat *lavfi_format = av_find_input_format("lavfi");

    if (lavfi_format == nullptr) {
        throw std::exception("[YMediaDestination] FFmpeg lavfi format not found");
    }

	_lavfi_audio_format_context = avformat_alloc_context();
	//aevalsrc=0:s=44100 //anullsrc=channel_layout=stereo:sample_rate=44100 //nb_samples=5500
    if (avformat_open_input(&_lavfi_audio_format_context, "aevalsrc=0", lavfi_format, nullptr)) {
        std::cout << "[YMediaDestination] Failed to open virtual input context" << std::endl;
        return;
    }

    if (avformat_find_stream_info(_lavfi_audio_format_context, nullptr) < 0) {
        std::cerr << "[YMediaDestination] Failed to retrieve input video stream information" << std::endl;
        return;
    }

    AVCodec *decoder = avcodec_find_decoder(AV_CODEC_ID_PCM_MULAW);
    if (decoder == nullptr) {
        std::cerr << "[YMediaDestination] Could not find decoder" << std::endl;
        return;
    }
    AVStream* out_stream = avformat_new_stream(_media_format_context, decoder);
    if (out_stream == nullptr) {
        std::cerr << "[YMediaDestination] Failed allocating output stream" << std::endl;
        return;
    }

    AVCodecContext* avCodecContext = avcodec_alloc_context3(decoder); 
	avCodecContext->codec_tag = 0;
	avCodecContext->bit_rate = 125 * 1024;
    avCodecContext->rc_min_rate = 44'100;
	avCodecContext->rc_max_rate = 44'100;
	avCodecContext->sample_rate = 44'100;
	avCodecContext->sample_fmt = AV_SAMPLE_FMT_FLTP;
	avCodecContext->channel_layout = AV_CH_LAYOUT_MONO;

	setAudioCodecContextOptions(avCodecContext);

    if (_media_format_context->oformat->flags & AVFMT_GLOBALHEADER) {
        avCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    if (avcodec_parameters_from_context(out_stream->codecpar, avCodecContext) < 0) {
        std::cerr << "[YMediaDestination] Failed to copy context input to output stream codec context" << std::endl;
        return;
    }

	av_dump_format(_lavfi_audio_format_context, 0, lavfi_format->name, 0);
}

void YMediaDestination::openVirtualVideoInput()
{
	//
}

AVPacket YMediaDestination::readVirtualVideoPacket()
{
	return AVPacket();
}

AVPacket YMediaDestination::readVirtualAudioPacket()
{
	AVPacket packet;
	if (av_read_frame(_lavfi_audio_format_context, &packet) < 0) {
        std::cerr << "[YMediaDestination] Failed to read lavfi audio" << std::endl;
		return AVPacket();
	}
	packet.stream_index = AVMEDIA_TYPE_AUDIO;
	packet.pos = -1;
	return packet;
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
