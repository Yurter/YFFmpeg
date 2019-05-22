#include "YMediaDestination.h"
#include <iostream>
#include <exception>

YMediaDestination::YMediaDestination(const std::string &mrl, YMediaPreset preset) :
	YAbstractMedia(mrl),
    _frame_index(0)
{
    switch (preset) {
    case Auto:
         guessOutputFromat();
         parseOutputFormat();
        break;
    case Silence:
        break;
    case YouTube:
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
    setVideoCodecName(avcodec_get_name(_output_format->video_codec));
    setAudioCodecName(avcodec_get_name(_output_format->audio_codec));
    setVideoCodecId(_output_format->video_codec);
    setAudioCodecId(_output_format->audio_codec);
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

            stampPacket(packet);

            if (packet.stream_index == _video_stream_index) {
                _frame_index++;
            }
            if (packet.stream_index == _audio_stream_index) {
                //
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

void YMediaDestination::stampPacket(AVPacket &packet)
{
    int64_t frame_duration = static_cast<int64_t>(1000.f / _frame_rate);
    packet.pts = _frame_index * frame_duration;
    packet.dts = packet.pts;
    packet.duration = (packet.stream_index = AVMEDIA_TYPE_VIDEO) ? frame_duration : packet.duration;
    packet.pos = -1;
}
