﻿#include "YContext.h"

YContext::YContext(const std::string& mrl, YMediaPreset preset) :
    _uid(INVALID_INT),
	_media_resource_locator(mrl),
    _opened(false),
    _reopening_after_failure(false),
    _reopening_timeout(INVALID_INT),
    _artificial_delay(DEFAULT_INT),
    _preset(preset),
    _format_context(nullptr)
{
    setName("YContext");
    setUid(utils::gen_context_uid());
}

YContext::~YContext()
{
    close();
}

YCode YContext::close()
{
    return_if(closed(), YCode::INVALID_CALL_ORDER);
//    _io_thread.quit(); //TODO вызывать в абстрактном класса
    avformat_free_context(_format_context);
    _opened = false;
    return YCode::OK;
}

bool YContext::opened() const
{
    return _opened;
}

bool YContext::closed() const
{
    return !_opened;
}

YCode YContext::createStream(YStream* new_stream)
{
    try_to(new_stream->init());
    new_stream->setUid(utils::gen_stream_uid(uid(), numberStream()));
    new_stream->parameters->setStreamIndex(numberStream());
    _streams.push_back(new_stream);
    return YCode::OK;
}

YCode YContext::createStream(YParameters* param)
{
    auto avstream = avformat_new_stream(_format_context, param->codec());
    return_if(not_inited_ptr(avstream), YCode::ERR);
    YStream* new_stream = nullptr;
    if (param->isVideo()) {
        auto video_param = dynamic_cast<YVideoParameters*>(param);
        new_stream = new YVideoStream(video_param);
        new_stream->setRaw(avstream);
    }
    if (param->isAudio()) {
        auto audio_param = dynamic_cast<YAudioParameters*>(param);
        new_stream = new YAudioStream(audio_param);
        new_stream->setRaw(avstream);
    }
    return createStream(new_stream);
}

YStream* YContext::bestStream(YMediaType type) //TODO разделить на два метода? определить алгоритм выбора
{
    int64_t best_stream_index = -1;
    if (type == YMediaType::MEDIA_TYPE_VIDEO) {
        int64_t best_resolution = 0;
        for (auto&& str : _streams) {
            if (str->isVideo()) {
                auto video_param = dynamic_cast<YVideoParameters*>(str->parameters);
                int64_t resolution = video_param->width() * video_param->height();
                if (resolution > best_resolution) {
                    best_resolution = resolution;
                    best_stream_index = str->index();
                }
            }
        }
    }
    if (type == YMediaType::MEDIA_TYPE_AUDIO) {
        int64_t best_bitrate = 0;
        for (auto&& str : _streams) {
            if (str->isVideo()) {
                auto audio_param = dynamic_cast<YAudioParameters*>(str->parameters);
                int64_t birtrate = audio_param->bitrate();
                if (birtrate > best_bitrate) {
                    best_bitrate = birtrate;
                    best_stream_index = str->index();
                }
            }
        }
    }
    return stream(best_stream_index);
}

void YContext::reopenAfterFailure(int64_t timeout)
{
    _reopening_after_failure = true;
    _reopening_timeout = timeout;
}

void YContext::setUid(int64_t uid)
{
    if (invalid_int(_uid)) { _uid = uid; }
}

int64_t YContext::uid() const
{
    return _uid;
}

YCode YContext::parseFormatContext()
{
    if (_format_context == nullptr) {
        log_error("Format context not inited. Parsing failed");
        return YCode::INVALID_INPUT;
    }

    {
//        _input_format = _format_context->iformat;
//        _output_format = _format_context->oformat;
    }

    for (int64_t i = 0; i < _format_context->nb_streams; i++) {
        AVStream* avstream = _format_context->streams[i];
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
            video_parameters->setContextUid(uid());
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
            /* crutch */ //TODO
            if (audio_parameters->channelLayout() == 0) {
                audio_parameters->setChannelLayout(uint64_t(av_get_default_channel_layout(int(audio_parameters->channels()))));
            }
            audio_parameters->setStreamIndex(i);
            audio_parameters->setTimeBase(avstream->time_base);
            audio_parameters->setContextUid(uid());
            createStream(new YAudioStream(avstream, audio_parameters));
            break;
        }
        default:
            log_warning("Unsupported media type founded: " << av_get_media_type_string(codec_type));
            break;
        }
    }

    return YCode::OK;

//    setDuration(FFMAX(_video_duration, _audio_duration)); //TODO
}

std::string YContext::mediaResourceLocator() const
{
    return _media_resource_locator;
}

AVFormatContext* YContext::mediaFormatContext() const
{
    return _format_context;
}

YStream* YContext::stream(int64_t index)
{
    if (size_t(index) < _streams.size()) {
        return _streams[size_t(index)];
    } else {
        return nullptr;
    }
}

int64_t YContext::numberStream() const
{
    return int64_t(_streams.size());
}
