#include "YAbstractMedia.h"

#include <iostream>

YAbstractMedia::YAbstractMedia(const std::string & mrl) :
	_media_resource_locator(mrl),
    _is_opened(false),
    _is_active(false),
    _reopening_after_failure(false),
    _reopening_timeout(0),
    _media_format_context(nullptr),
    _input_format(nullptr),
    _output_format(nullptr),
    _video_available(false),
    _audio_available(false),
    _video_stream_index(-1),
    _audio_stream_index(-1),
    _duration(0),
    _video_codec_id(AV_CODEC_ID_NONE),
    _video_codec_name("none"),
	_width(0),
	_height(0),
    _aspect_ratio({-1,-1}),
	_frame_rate(0.0f),
    _video_bitrate(0),
    _video_duration(0),
    _audio_codec_id(AV_CODEC_ID_NONE),
    _audio_codec_name("none"),
    _audio_bitrate(0),
    _audio_duration(0)
{
    //
}

YAbstractMedia::~YAbstractMedia()
{
    avformat_free_context(_media_format_context);
}

bool YAbstractMedia::close()
{
    if (!_is_opened) { return false; }
    if (_thread.joinable()) {
        _is_active = false;
        _thread.join();
    }
    return true;
}

void YAbstractMedia::parseFormatContext()
{
    if (_media_format_context == nullptr) {
        std::cerr << "[YAbstractMedia] Format context not inited. Parsing failed." << std::endl;
        return;
    }

    {
        _input_format = _media_format_context->iformat;
        _output_format = _media_format_context->oformat;
    }

    for (int64_t i = 0; i < _media_format_context->nb_streams; i++) {
		AVStream* in_stream = _media_format_context->streams[i];
        auto codecpar = in_stream->codecpar;
        auto codec_type = codecpar->codec_type;

        switch (codec_type) {
        case AVMEDIA_TYPE_VIDEO: {
            setVideoCodecId(codecpar->codec_id);
            setVideoCodecName(avcodec_get_name(codecpar->codec_id));
            setWidth(codecpar->width);
            setHeight(codecpar->height);
            setAspectRatio({-1,-1}); //TODO
            setVideoDuration(in_stream->duration);
            setFrameRate(in_stream->avg_frame_rate); // ? -> r_frame_rate
            setVideoBitrate(codecpar->bit_rate);
            _video_available = true;
            _video_stream_index = i;
            break;
        }
        case AVMEDIA_TYPE_AUDIO: {
            setAudioCodecId(codecpar->codec_id);
            setAudioCodecName(avcodec_get_name(codecpar->codec_id));
            setSampleRate(codecpar->sample_rate);
//            setSampleFormat(codecpar->)
            setAudioBitrate(codecpar->bit_rate);
            setAudioChanelsLayout(codecpar->channel_layout);
            setAudioChanels(codecpar->channels);
            _audio_available = true;
            _audio_stream_index = i;
            break;
        }
        default:
            std::cerr << "[YAbstractMedia] Unsupported media type founded: " << av_get_media_type_string(codec_type) << std::endl;
            break;
        }
    }

    setDuration(MAX(_video_duration, _audio_duration));
}

std::string YAbstractMedia::guessFormatShortName()
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

AVInputFormat *YAbstractMedia::inputFormat() const
{
    return _input_format;
}

AVOutputFormat *YAbstractMedia::outputFormat() const
{
    return _output_format;
}

int64_t YAbstractMedia::width() const
{
	return _width;
}

int64_t YAbstractMedia::height() const
{
    return _height;
}

AVRational YAbstractMedia::aspectRatio() const
{
    return _aspect_ratio;
}

int64_t YAbstractMedia::sampleRate() const
{
    return _sample_rate;
}

AVSampleFormat YAbstractMedia::sampleFormat() const
{
    return _sample_format;
}

int64_t YAbstractMedia::audioBitrate() const
{
    return _audio_bitrate;
}

uint64_t YAbstractMedia::audioChanelsLayout() const
{
    return _chanels_layout;
}

int64_t YAbstractMedia::audioChanels() const
{
    return _chanels;
}

uint64_t YAbstractMedia::frameRate() const
{
	return _frame_rate;
}

int64_t YAbstractMedia::duration() const
{
    return _duration;
}

int64_t YAbstractMedia::videoDuration() const
{
    return _video_duration;
}

int64_t YAbstractMedia::audioDuration() const
{
    return _audio_duration;
}

std::string YAbstractMedia::videoCodecName() const
{
	return _video_codec_name;
}

std::string YAbstractMedia::audioCodecName() const
{
    return _audio_codec_name;
}

AVCodecID YAbstractMedia::videoCodecId() const
{
    return _video_codec_id;
}

AVCodecID YAbstractMedia::audioCodecId() const
{
    return _audio_codec_id;
}

bool YAbstractMedia::videoAvailable() const
{
	return _video_available;
}

bool YAbstractMedia::audioAvailable() const
{
    return _audio_available;
}

int64_t YAbstractMedia::videoStreamIndex() const
{
    return _video_stream_index;
}

int64_t YAbstractMedia::audioStreamIndex() const
{
    return _audio_stream_index;
}

void YAbstractMedia::setReopeningTimeout(uint64_t reopening_timeout)
{
    _reopening_timeout = reopening_timeout;
}

void YAbstractMedia::setWidth(int64_t width)
{
    _width = width - (width % 2);
}

void YAbstractMedia::setHeight(int64_t height)
{
    _height = height - (height % 2);
}

void YAbstractMedia::setDuration(int64_t duration)
{
    _duration = duration;
}

void YAbstractMedia::setVideoDuration(int64_t video_duration)
{
    _video_duration = video_duration;
}

void YAbstractMedia::setAudioDuration(int64_t audio_duration)
{
    _audio_duration = audio_duration;
}

void YAbstractMedia::setAspectRatio(AVRational aspect_ratio)
{
    _aspect_ratio = aspect_ratio;
}

void YAbstractMedia::setFrameRate(uint64_t frame_rate)
{
    _frame_rate = frame_rate;
}

void YAbstractMedia::setFrameRate(AVRational frame_rate)
{
    if (frame_rate.den == 0) {
        setFrameRate(0);
    } else {
        setFrameRate(static_cast<uint64_t>(frame_rate.num / frame_rate.den));
    }
}

void YAbstractMedia::setVideoCodecName(std::string video_codec_name)
{
    if (video_codec_name.empty())   { return; }
    if (video_codec_name == "none") { return; }
    if (video_codec_name != "h264") { return; } //TODO
    _video_codec_name = video_codec_name;
    _video_available = true;
}

void YAbstractMedia::setAudioCodecName(std::string audio_codec_name)
{
    if (audio_codec_name.empty())   { return; }
    if (audio_codec_name == "none") { return; }
    _audio_codec_name = audio_codec_name;
    _audio_available = true;
}

void YAbstractMedia::setVideoCodecId(AVCodecID video_codec_id)
{
    _video_codec_id = video_codec_id;
}

void YAbstractMedia::setAudioCodecId(AVCodecID audio_codec_id)
{
    _audio_codec_id = audio_codec_id;
}

void YAbstractMedia::setSampleRate(int64_t sample_rate)
{
    _sample_rate = sample_rate;
}

void YAbstractMedia::setSampleFormat(AVSampleFormat sample_format)
{
    _sample_format = sample_format;
}

void YAbstractMedia::setVideoBitrate(int64_t video_bitrate)
{
    _video_bitrate = video_bitrate;
}

void YAbstractMedia::setAudioBitrate(int64_t audio_bitrate)
{
    _audio_bitrate = audio_bitrate;
}

void YAbstractMedia::setAudioChanels(int64_t chanels)
{
    _chanels = chanels;
}

void YAbstractMedia::setAudioChanelsLayout(uint64_t chanels_layout)
{
    _chanels_layout = chanels_layout;
}
