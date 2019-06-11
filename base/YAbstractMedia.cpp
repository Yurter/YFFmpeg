#include "YAbstractMedia.h"

#include <iostream>

YAbstractMedia::YAbstractMedia(const std::string & mrl) :
	_media_resource_locator(mrl),
    _opened(false),
    _reopening_after_failure(false),
    _reopening_timeout(-1),         //TODO
    _close_after_failure(false),
    _close_timeout(-1),             //TODO
    _artificial_delay(0),
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
//    _io_thread.quit(); //TODO вызывать в абстрактном класса
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
        AVStream* avstream = _media_format_context->streams[i];
        auto codec = avstream->codec;
        auto codecpar = avstream->codecpar;
        auto codec_type = codecpar->codec_type;

        switch (codec_type) {
        case AVMEDIA_TYPE_VIDEO: {
            video_parameters.setCodec(codecpar->codec_id);
            video_parameters.setWidth(codecpar->width);
            video_parameters.setHeight(codecpar->height);
            video_parameters.setAspectRatio({ -1, -1 }); //TODO
            video_parameters.setDuration(avstream->duration);
            video_parameters.setFrameRate(avstream->avg_frame_rate); // ? -> r_frame_rate
            video_parameters.setBitrate(codecpar->bit_rate);
            video_parameters.setPixelFormat(codec->pix_fmt);
            video_parameters.setStreamIndex(i);
            video_parameters.setTimeBase(avstream->time_base);
            video_parameters.setAvailable(true);
            _streams.push_back({ avstream, YMediaType::MEDIA_TYPE_VIDEO });
            break;
        }
        case AVMEDIA_TYPE_AUDIO: {
            audio_parameters.setCodec(codecpar->codec_id);
            audio_parameters.setSampleRate(codecpar->sample_rate);
            audio_parameters.setSampleFormat(codec->sample_fmt);
            audio_parameters.setDuration(avstream->duration);
            audio_parameters.setBitrate(codecpar->bit_rate);
            audio_parameters.setChanelsLayout(codecpar->channel_layout);
            audio_parameters.setChanels(codecpar->channels);
            audio_parameters.setStreamIndex(i);
            video_parameters.setTimeBase(avstream->time_base);
            audio_parameters.setAvailable(true);
            _streams.push_back({ avstream, YMediaType::MEDIA_TYPE_AUDIO });
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
    std::lock_guard<std::mutex> lock(_media_format_context_mutex); //TODO
    return _media_format_context;
}

YStream* YAbstractMedia::stream(uint64_t index)
{
    if (index < _streams.size()) {
        return &_streams[index];
    } else {
        return nullptr;
    }
}
