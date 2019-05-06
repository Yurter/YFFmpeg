#include "YAbstractMedia.h"

YAbstractMedia::YAbstractMedia(const std::string & mrl) :
	_media_resource_locator(mrl),
    _is_active(false),
    _is_opened(false),
	_media_format_context(nullptr),
	_lavfi_video_format_context(nullptr),
    _lavfi_audio_format_context(nullptr),
	_width(0),
	_height(0),
	_duration(0),
	_frame_rate(0.0f),
    _video_codec_id(AV_CODEC_ID_NONE),
    _audio_codec_id(AV_CODEC_ID_NONE),
	_video_codec_name("undefined"),
	_audio_codec_name("undefined"),
	_video_available(false),
	_audio_available(false)
{
    //
}

YAbstractMedia::~YAbstractMedia()
{
    //
}

void YAbstractMedia::getInfo()
{
    if (_media_format_context == nullptr) { return; }

	if (_media_format_context->iformat != nullptr) {
		_format = _media_format_context->iformat->name;
	}
	if (_media_format_context->oformat != nullptr) {
		_format = _media_format_context->oformat->name;
	}

	for (uint64_t i = 0; i < _media_format_context->nb_streams; i++) {
		AVStream* in_stream = _media_format_context->streams[i];
		auto codec_type = in_stream->codecpar->codec_type;

		if (codec_type == AVMEDIA_TYPE_VIDEO) {
            _width = static_cast<uint64_t>(in_stream->codecpar->width);
            _height = static_cast<uint64_t>(in_stream->codecpar->height);
            _duration = static_cast<uint64_t>(in_stream->duration);
            if (in_stream->avg_frame_rate.den == 0) {
                _frame_rate = -1.f;
            } else {
                _frame_rate = in_stream->avg_frame_rate.num / in_stream->avg_frame_rate.den;
            }
			AVCodec *decoder = avcodec_find_decoder(in_stream->codecpar->codec_id);
			_video_codec_name = decoder ? decoder->name : "undefined";
			_video_available = true;
		}

		if (codec_type == AVMEDIA_TYPE_AUDIO) {
			AVCodec *decoder = avcodec_find_decoder(in_stream->codecpar->codec_id);
			_audio_codec_name = decoder ? decoder->name : "undefined";
			_audio_available = true;
        }
	}
}

std::string YAbstractMedia::guessFormatName()
{
	if (_media_resource_locator.find("rtmp://") != std::string::npos) {
		return std::string("flv");
	}
	return std::string();
}

bool YAbstractMedia::isActive() const
{
	return _is_active;
}

std::string YAbstractMedia::mediaResourceLocator() const
{
    return _media_resource_locator;
}

AVFormatContext *YAbstractMedia::mediaFormatContext() const
{
    return _media_format_context;
}

uint64_t YAbstractMedia::width() const
{
	return _width;
}

uint64_t YAbstractMedia::height() const
{
	return _height;
}

float YAbstractMedia::frameRate() const
{
	return _frame_rate;
}

uint64_t YAbstractMedia::duration() const
{
	return _duration;
}

std::string YAbstractMedia::format() const
{
	return _format;
}

std::string YAbstractMedia::videoCodec() const
{
	return _video_codec_name;
}

std::string YAbstractMedia::audioCodec() const
{
	return _audio_codec_name;
}

bool YAbstractMedia::videoStreamAvailable() const
{
	return _video_available;
}

bool YAbstractMedia::audioStreamAvailable() const
{
	return _audio_available;
}

void YAbstractMedia::setWidth(uint64_t width)
{
	_width = width;
}

void YAbstractMedia::setHeight(uint64_t height)
{
	_height = height;
}

void YAbstractMedia::setFrameRate(float frame_rate)
{
    _frame_rate = frame_rate;
}

void YAbstractMedia::setFormat(std::string format)
{
    _format = format;
}

void YAbstractMedia::setVideoCodecName(std::string video_codec_name)
{
    _video_codec_name = video_codec_name;
}

void YAbstractMedia::setAudioCodecName(std::string audio_codec_name)
{
    _audio_codec_name = audio_codec_name;
}
