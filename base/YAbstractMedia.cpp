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

YCode YAbstractMedia::createStream(YStream new_stream)
{
    new_stream.setUid(utils::gen_stream_uid(uid(), numberStream()));
    _streams.push_back(new_stream);
    return YCode::OK;
}

//YCode YAbstractMedia::createStream(AVCodecID codec_id)
//{
//    YStream new_stream;
//    new_stream.setRaw(avformat_new_stream(_media_format_context, codec_id));
//    if (not_inited_ptr(new_stream.raw())) {
//       log_error("Failed allocating output stream");
//       return YCode::ERR;
//    }
//    if (avcodec_parameters_from_context(new_stream.codecParameters(), codec_context) < 0) {
//       log_error("Failed to copy context input to output stream codec context");
//       return YCode::ERR;
//    }
//    {
//        _streams.push_back(new_stream);
//        return YCode::OK;
//    }
//}

//YCode YAbstractMedia::createStream(YMediaType type, YParameters parametres)
//{
//    switch (type) {
//    case YMediaType::MEDIA_TYPE_VIDEO:
//        _streams.push_back(YVideoStream(parametres));
//        return YCode::OK;
//    case YMediaType::MEDIA_TYPE_AUDIO:
//        _streams.push_back(YAudioStream(parametres));
//        return YCode::OK;
//    case YMediaType::MEDIA_TYPE_UNKNOWN:
//        return YCode::INVALID_INPUT;
//    }
//}

//YCode YAbstractMedia::createDummyStream(YMediaType type, YParameters parametres)
//{
//    switch (type) {
//    case YMediaType::MEDIA_TYPE_VIDEO:
//        _streams.push_back(YVideoStream(parametres));
//        return YCode::OK;
//    case YMediaType::MEDIA_TYPE_AUDIO:
//        _streams.push_back(YAudioStream(parametres));
//        return YCode::OK;
//    case YMediaType::MEDIA_TYPE_UNKNOWN:
//        return YCode::INVALID_INPUT;
//    }
//}

//YCode YAbstractMedia::createStream(AVCodecContext* codec_context)
//{
//    YStream new_stream;
//    new_stream.setRaw(avformat_new_stream(_media_format_context, codec_context->codec));
//    if (not_inited_ptr(new_stream.raw())) {
//       log_error("Failed allocating output stream");
//       return YCode::ERR;
//    }
//    if (avcodec_parameters_from_context(new_stream.codecParameters(), codec_context) < 0) {
//       log_error("Failed to copy context input to output stream codec context");
//       return YCode::ERR;
//    }
//    {
//        _streams.push_back(new_stream);
//        return YCode::OK;
//    }
//}

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
            YVideoParameters video_parameters;
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
            createStream(YVideoStream(avstream, video_parameters));
            break;
        }
        case AVMEDIA_TYPE_AUDIO: {
            YAudioParameters audio_parameters;
            audio_parameters.setCodec(codecpar->codec_id);
            audio_parameters.setSampleRate(codecpar->sample_rate);
            audio_parameters.setSampleFormat(codec->sample_fmt);
            audio_parameters.setDuration(avstream->duration);
            audio_parameters.setBitrate(codecpar->bit_rate);
            audio_parameters.setChannelLayout(codecpar->channel_layout);
            audio_parameters.setChanels(codecpar->channels);
            audio_parameters.setStreamIndex(i);
            audio_parameters.setTimeBase(avstream->time_base);
            createStream(YAudioStream(avstream, audio_parameters));
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

YStream* YAbstractMedia::stream(int64_t index)
{
    if (size_t(index) < _streams.size()) {
        return &_streams[size_t(index)];
    } else {
        return nullptr;
    }
}

int64_t YAbstractMedia::numberStream() const
{
    return int64_t(_streams.size());
}

YCode YAbstractMedia::createContext()
{
    std::string output_format_name = guessFormatShortName();
    const char *format_name = output_format_name.empty() ? nullptr : output_format_name.c_str();
    if (avformat_alloc_output_context2(&_media_format_context, nullptr, format_name, _media_resource_locator.c_str()) < 0) {
        log_error("Failed to alloc output context.");
        return YCode::ERR;
    }
    return YCode::OK;
}

YCode YAbstractMedia::attachStreams()
{
    for (auto&& str : _streams) {
        auto avstream = avformat_new_stream(_media_format_context, nullptr);
        return_if(not_inited_ptr(avstream), YCode::ERR);
//        utils::codecpar_from_parameters(avstream->codecpar, str.parameters);
        utils::init_codecpar(avstream->codecpar, str.parameters.codec());
        str.setRaw(avstream);
    }
    return YCode::OK;
}
