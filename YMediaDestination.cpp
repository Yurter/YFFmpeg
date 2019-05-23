#include "YMediaDestination.h"
#include <iostream>
#include <exception>

YMediaDestination::YMediaDestination(const std::string &mrl, YMediaPreset preset) :
	YAbstractMedia(mrl),
    _video_packet_index(0),
    _audio_packet_index(0)
{
    switch (preset) {
    case Auto:
         guessOutputFromat();
         parseOutputFormat();
        break;
    case Silence:
        break;
    case YouTube:
        /* Video */
        setWidth(1920);
        setHeight(1080);
        setAspectRatio({16,9});
        setFrameRate(30);
        setVideoBitrate(400'000);
        setVideoCodec("libx264");
        /* Audio */
        setSampleRate(44'100);
        setSampleFormat(AV_SAMPLE_FMT_FLTP);
        setAudioBitrate(128 * 1024);
        setAudioChanelsLayout(AV_CH_LAYOUT_STEREO);
        setAudioChanels(2);
        //setAudioCodec("aac");
        setAudioCodec("mp3");
//        setAudioCodec("wav");
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
    AVStream* out_stream = avformat_new_stream(_media_format_context, stream_codec_context->codec);
    if (out_stream == nullptr) {
        std::cerr << "[YMediaDestination] Failed allocating output stream" << std::endl;
        return false;
    }

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

    std::cout << "[YMediaDestination] Created stream: " << stream_codec_context->codec->name << std::endl;
    return true;
}

bool YMediaDestination::open()
{
//    std::cout << "[YMediaDestination] Opening" << std::endl;

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
    run();
    return true;
}

bool YMediaDestination::close()
{
    if (!_is_opened) { return false; }
    av_write_trailer(_media_format_context);

    YAbstractMedia::close();

    return true;
}

void YMediaDestination::writePacket(AVPacket packet)
{
    queuePacket(packet);
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
    setVideoCodec(_output_format->video_codec);
    setAudioCodec(_output_format->audio_codec);
}

void YMediaDestination::run()
{
    _thread = std::thread([this](){
        while (_is_opened) {
            AVPacket packet;
            if (!getPacket(packet)) {
                std::cerr << "[YMediaDestination] Buffer is empty." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            if (!stampPacket(packet)) {
                std::cerr << "[YMediaDestination] stampPacket failed" << std::endl;
                break;
            }

            if (av_interleaved_write_frame(_media_format_context, &packet) < 0) {
                std::cerr << "[YMediaDestination] Error muxing packet" << std::endl;
                break;
            } else {
                std::cerr << "[YMediaDestination] Writed" << std::endl;
            }
        }
    });
}

bool YMediaDestination::stampPacket(AVPacket &packet)
{
    if (packet.stream_index == _video_stream_index) {
        packet.pts = _video_packet_index;
        packet.dts = _video_packet_index;
//        packet.duration
        packet.pos = -1;
        _video_packet_index++;
        return false; //TODO: duration
    }
    if (packet.stream_index == _audio_stream_index) {
        packet.pts = _audio_packet_index;
        packet.dts = _audio_packet_index;
        packet.pos = -1;
        _audio_packet_index++;
        return true;
    }
    return false;
}
