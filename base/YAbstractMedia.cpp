#include "YAbstractMedia.h"

#include <iostream>

YAbstractMedia::YAbstractMedia(const std::string & mrl) :
	_media_resource_locator(mrl),
    _opened(false),
    _running(false),
    _reopening_after_failure(false),
    _reopening_timeout(-1),         //TODO
    _close_after_failure(false),
    _close_timeout(-1),             //TODO
    _artificial_delay(0),
    _packet_queue_capacity(100),
    _media_format_context(nullptr)
{
    //
}

YAbstractMedia::~YAbstractMedia()
{
    close();
}

bool YAbstractMedia::close()
{
    if (!_opened) { return false; }
    _packet_queue = std::queue<YPacket>();
    avformat_free_context(_media_format_context);
    _opened = false;
    return true;
}

bool YAbstractMedia::opened() const
{
    return _opened;
}

bool YAbstractMedia::closed() const
{
    return !_opened;
}

void YAbstractMedia::parseFormatContext()
{
    if (_media_format_context == nullptr) {
        std::cerr << "[YAbstractMedia] Format context not inited. Parsing failed." << std::endl;
        return;
    }

    {
//        _input_format = _media_format_context->iformat;
//        _output_format = _media_format_context->oformat;
    }

    for (int64_t i = 0; i < _media_format_context->nb_streams; i++) {
        AVStream* stream = _media_format_context->streams[i];
        auto codec = stream->codec;
        auto codecpar = stream->codecpar;
        auto codec_type = codecpar->codec_type;

        switch (codec_type) {
        case AVMEDIA_TYPE_VIDEO: {
            video_parameters.setCodec(codecpar->codec_id);
            video_parameters.setWidth(codecpar->width);
            video_parameters.setHeight(codecpar->height);
            video_parameters.setAspectRatio({-1,-1}); //TODO
            video_parameters.setDuration(stream->duration);
            video_parameters.setFrameRate(stream->avg_frame_rate); // ? -> r_frame_rate
            video_parameters.setBitrate(codecpar->bit_rate);
            video_parameters.setPixelFormat(codec->pix_fmt);
            video_parameters.setStreamIndex(i);
            video_parameters.setTimeBase(stream->time_base);
            video_parameters.setAvailable(true);
            break;
        }
        case AVMEDIA_TYPE_AUDIO: {
            audio_parameters.setCodec(codecpar->codec_id);
            audio_parameters.setSampleRate(codecpar->sample_rate);
            audio_parameters.setSampleFormat(codec->sample_fmt);
            audio_parameters.setDuration(stream->duration);
            audio_parameters.setBitrate(codecpar->bit_rate);
            audio_parameters.setChanelsLayout(codecpar->channel_layout);
            audio_parameters.setChanels(codecpar->channels);
            audio_parameters.setStreamIndex(i);
            video_parameters.setTimeBase(stream->time_base);
            audio_parameters.setAvailable(true);
            break;
        }
        default:
            std::cerr << "[YAbstractMedia] Unsupported media type founded: " << av_get_media_type_string(codec_type) << std::endl;
            break;
        }
    }

//    setDuration(FFMAX(_video_duration, _audio_duration));
}

std::string YAbstractMedia::guessFormatShortName()
{
	if (_media_resource_locator.find("rtmp://") != std::string::npos) {
		return std::string("flv");
	}
    if (_media_resource_locator.find("aevalsrc") != std::string::npos) {
        return std::string("lavfi");
    }
	return std::string();
}

std::string YAbstractMedia::mediaResourceLocator() const
{
    return _media_resource_locator;
}

AVFormatContext *YAbstractMedia::mediaFormatContext() const
{
    return _media_format_context;
}

AVStream *YAbstractMedia::stream(int64_t index)
{
    return _media_format_context->streams[index];
}

void YAbstractMedia::queuePacket(YPacket packet)
{
    std::lock_guard<std::mutex> lock(_packet_queue_mutex);
    _packet_queue.push(packet);
}

bool YAbstractMedia::getPacket(YPacket &packet)
{
    std::lock_guard<std::mutex> lock(_packet_queue_mutex);
    if (_packet_queue.empty()) { return false; }
    packet = _packet_queue.front();
    _packet_queue.pop();
    return true;
}

void YAbstractMedia::stopThread()
{
    if (!_running) { return; }
    _running = false;
    if (_thread.joinable()) { _thread.join(); }
}
