#include "YAbstractMedia.h"

YAbstractMedia::YAbstractMedia(const std::string& mrl) :
    _uid(INVALID_INT),
	_media_resource_locator(mrl),
    _opened(false),
    _reopening_after_failure(false),
    _reopening_timeout(INVALID_INT),
    _artificial_delay(DEFAULT_INT),
    _media_format_context(nullptr)
{
    setName("YAbstractMedia");
    setUid(utils::gen_context_uid());
//    createContext();
}

YAbstractMedia::~YAbstractMedia()
{
    close();
}

YCode YAbstractMedia::close()
{
    return_if(closed(), YCode::INVALID_CALL_ORDER);
//    _io_thread.quit(); //TODO вызывать в абстрактном класса
    avformat_free_context(_media_format_context);
    _opened = false;
    return YCode::OK;
}

bool YAbstractMedia::opened() const
{
    return _opened;
}

bool YAbstractMedia::closed() const
{
    return !_opened;
}

YCode YAbstractMedia::createStream(YStream* new_stream)
{
    new_stream->setUid(utils::gen_stream_uid(uid(), numberStream()));
    _streams.push_back(new_stream);
    return YCode::OK;
}

void YAbstractMedia::setUid(int64_t uid)
{
    if (invalid_int(_uid)) { _uid = uid; }
}

int64_t YAbstractMedia::uid() const
{
    return _uid;
}

YCode YAbstractMedia::parseFormatContext()
{
    if (_media_format_context == nullptr) {
        log_error("Format context not inited. Parsing failed");
        return YCode::INVALID_INPUT;
    }

    {
//        _input_format = _media_format_context->iformat;
//        _output_format = _media_format_context->oformat;
    }

    for (int64_t i = 0; i < _media_format_context->nb_streams; i++) { //TODO video_parameters | audio_parameters -> setAvailable
        AVStream* avstream = _media_format_context->streams[i];
        auto codec = avstream->codec;
        auto codecpar = avstream->codecpar;
        auto codec_type = codecpar->codec_type;

        switch (codec_type) {//TODO
        case AVMEDIA_TYPE_VIDEO: {
            auto video_parameters = new YVideoParameters;
            video_parameters->setCodec(codecpar->codec_id);
            video_parameters->setWidth(codecpar->width);
            video_parameters->setHeight(codecpar->height);
            video_parameters->setAspectRatio({ -1, -1 }); //TODO
            video_parameters->setDuration(avstream->duration);
            video_parameters->setFrameRate(avstream->avg_frame_rate); // ? -> r_frame_rate
            video_parameters->setBitrate(codecpar->bit_rate);
            video_parameters->setPixelFormat(codec->pix_fmt);
            video_parameters->setStreamIndex(i);
            video_parameters->setTimeBase(avstream->time_base);
            createStream(new YVideoStream(avstream, video_parameters));
            break;
        }
        case AVMEDIA_TYPE_AUDIO: {
            auto audio_parameters = new YAudioParameters;
            audio_parameters->setCodec(codecpar->codec_id);
            audio_parameters->setSampleRate(codecpar->sample_rate);
            audio_parameters->setSampleFormat(codec->sample_fmt);
            audio_parameters->setDuration(avstream->duration);
            audio_parameters->setBitrate(codecpar->bit_rate);
            audio_parameters->setChannelLayout(codecpar->channel_layout);
            audio_parameters->setChannels(codecpar->channels);
            audio_parameters->setStreamIndex(i);
            audio_parameters->setTimeBase(avstream->time_base);
            createStream(new YAudioStream(avstream, audio_parameters));
            break;
        }
        default:
            log_warning("Unsupported media type founded: " << av_get_media_type_string(codec_type));
            break;
        }
    }

    return YCode::OK;

//    setDuration(FFMAX(_video_duration, _audio_duration));
}

std::string YAbstractMedia::guessFormatShortName()
{
    if (_media_resource_locator.find("rtsp://") != std::string::npos) {
        return std::string("rtsp");
    }
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

AVFormatContext* YAbstractMedia::mediaFormatContext() const
{
    return _media_format_context;
}

YStream* YAbstractMedia::stream(int64_t index)
{
    if (size_t(index) < _streams.size()) {
        return _streams[size_t(index)];
    } else {
        return nullptr;
    }
}

int64_t YAbstractMedia::numberStream() const
{
    return int64_t(_streams.size());
}

YCode YAbstractMedia::attachStreams() //TODO
{
    for (auto&& str : _streams) {
        if (not_inited_ptr(str->raw())) {
            auto avstream = avformat_new_stream(_media_format_context, nullptr);
            return_if(not_inited_ptr(avstream), YCode::ERR);
            str->setRaw(avstream);
        }
        utils::init_codecpar(str->codecParameters(), str->parameters->codec());
        utils::parameters_to_avcodecpar(str->parameters, str->codecParameters());
    }
    return YCode::OK;
}
