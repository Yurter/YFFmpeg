#include "FormatContext.hpp"
#include "core/utils.hpp"

namespace fpp {

    FormatContext::FormatContext(const std::string mrl, Object* stream_context, IOType preset) :
        _uid(utils::gen_uid())
        , _media_resource_locator(mrl)
        , _opened(false)
        , _stream_context(stream_context)
        , _reopening_after_failure(false)
        , _reopening_timeout(INVALID_INT)
        , _artificial_delay(DEFAULT_INT)
        , _preset(preset)
        , _current_interrupter(InterruptedProcess::None, this)
        , _format_context(nullptr)
    {
        setName("FormatContext");
    }

    FormatContext::~FormatContext() {
//        try_throw(close()); //TODO
    }

    Code FormatContext::close() {
        return_if(closed(), Code::OK);
        try_to(closeContext());
        avformat_free_context(_format_context);
        setOpened(false);
        return Code::OK;
    }

    bool FormatContext::opened() const {
        return _opened;
    }

    bool FormatContext::closed() const {
        return !_opened;
    }

    void FormatContext::setOpened(bool opened) {
        _opened = opened;
    }

    IOType FormatContext::preset() const {
        return _preset;
    }

    bool FormatContext::preset(IOType value) const {
        return _preset == value;
    }

    Code FormatContext::open() {
        return_if(opened(), Code::OK);
        if_not(inited()) { try_to(init()); } //TODO оставить изменить?
        setInteruptCallback(InterruptedProcess::Opening);
        try_to(openContext());
        resetInteruptCallback();
        setOpened(true);
        return Code::OK;
    }

    Code FormatContext::createStream(Stream* new_stream) {
//        try_to(new_stream->init()); // перенёс на момент открытия контекста
        new_stream->setContext(_stream_context);
        new_stream->setUid(utils::gen_stream_uid(uid(), numberStream()));
        new_stream->params->setStreamIndex(numberStream());
        new_stream->params->setContextUid(uid());
        _streams.push_back(new_stream);
        return Code::OK;
    }

    Code FormatContext::createStream(Parameters* param) {
//        return_if(not_inited_codec_id(param->codecId()), Code::INVALID_INPUT); //Нужна ли проверка?..
        auto avstream = avformat_new_stream(_format_context, param->codec());
        return_if(not_inited_ptr(avstream), Code::ERR);
        Stream* new_stream = nullptr;
        if (param->isVideo()) {
            auto video_param = dynamic_cast<VideoParameters*>(param);
            new_stream = new VideoStream(video_param);
            new_stream->setRaw(avstream);
        }
        if (param->isAudio()) {
            auto audio_param = dynamic_cast<AudioParameters*>(param);
            new_stream = new AudioStream(audio_param);
            new_stream->setRaw(avstream);
        }
        return createStream(new_stream);
    }

    Stream* FormatContext::bestStream(MediaType type) {
        return utils::find_best_stream(streams(type));
    }

    void FormatContext::reopenAfterFailure(int64_t timeout) {
        _reopening_after_failure = true;
        _reopening_timeout = timeout;
    }

    void FormatContext::setUid(int64_t uid) {
        if (invalid_int(_uid)) { _uid = uid; }
    }

    void FormatContext::setInteruptCallback(InterruptedProcess process) {
        _current_interrupter.interrupted_process = process;
        _current_interrupter.chronometer.reset_timepoint();
        _format_context->interrupt_callback.callback =
                &FormatContext::interrupt_callback;
        _format_context->interrupt_callback.opaque =
                &_current_interrupter;
    }

    void FormatContext::resetInteruptCallback() {
        _current_interrupter.interrupted_process = InterruptedProcess::None;
        _format_context->interrupt_callback.callback = nullptr; /* Бессмысленно :( */
        _format_context->interrupt_callback.opaque = nullptr;   /* Бессмысленно :( */
    }

    int FormatContext::interrupt_callback(void* opaque) {
        auto interrupter = reinterpret_cast<Interrupter*>(opaque);
        switch (interrupter->interrupted_process) {
        case InterruptedProcess::None:
            return 0;
        case InterruptedProcess::Opening: {
            int opening_timeout_ms = 5000;
            if (interrupter->chronometer.elapsed_milliseconds() > opening_timeout_ms) {
                static_log_error("interrupt_callback", "Opening timed out: " << opening_timeout_ms);
                return 1;
            }
            return 0;
        }
        case InterruptedProcess::Closing:
            static_log_error("interrupt_callback", "InterruptedProcess::Closing is not implemeted");
            return -1;
        case InterruptedProcess::Reading:
            static_log_error("interrupt_callback", "InterruptedProcess::Reading is not implemeted");
            return -1;
        case InterruptedProcess::Writing:
            static_log_error("interrupt_callback", "InterruptedProcess::Writing is not implemeted");
            return -1;
        }
        static_log_warning("interrupt_callback", "opaque = " << opaque);
        return 0;
    }

    int64_t FormatContext::uid() const {
        return _uid;
    }

    Code FormatContext::parseFormatContext() { //TODO перенести код в Stream::parseParametres()
        if (_format_context == nullptr) {
            log_error("Format context not inited. Parsing failed");
            return Code::INVALID_INPUT;
        }

        for (int64_t i = 0; i < _format_context->nb_streams; i++) {
            AVStream* avstream = _format_context->streams[i];
            auto codec = avstream->codec;
            auto codecpar = avstream->codecpar;
            auto codec_type = codecpar->codec_type;

            switch (codec_type) {//TODO
            case AVMEDIA_TYPE_VIDEO: {
                auto video_parameters = new VideoParameters;
                video_parameters->setCodec(codecpar->codec_id, CodecType::Decoder);
                video_parameters->setWidth(codecpar->width);
                video_parameters->setHeight(codecpar->height);
                video_parameters->setAspectRatio(codecpar->sample_aspect_ratio);
                video_parameters->setDuration(avstream->duration);
                video_parameters->setFrameRate(avstream->avg_frame_rate); // ? -> r_frame_rate
                video_parameters->setBitrate(codecpar->bit_rate);
                video_parameters->setPixelFormat(codec->pix_fmt);
                video_parameters->setStreamIndex(i);
                video_parameters->setTimeBase(avstream->time_base);
                video_parameters->setContextUid(uid());
                try_to(createStream(new VideoStream(avstream, video_parameters)));
                break;
            }
            case AVMEDIA_TYPE_AUDIO: {
                auto audio_parameters = new AudioParameters;
                audio_parameters->setCodec(codecpar->codec_id, CodecType::Decoder);
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
                try_to(createStream(new AudioStream(avstream, audio_parameters)));
                break;
            }
            default:
                log_warning("Unsupported media type founded: " << av_get_media_type_string(codec_type));
                break;
            }
        }

        return Code::OK;

    //    setDuration(FFMAX(_video_duration, _audio_duration)); //TODO
    }

    std::string FormatContext::mediaResourceLocator() const {
        return _media_resource_locator;
    }

    AVFormatContext* FormatContext::mediaFormatContext() const {
        return _format_context;
    }

    Stream* FormatContext::stream(int64_t index) {
        if (size_t(index) < _streams.size()) {
            return _streams[size_t(index)];
        } else {
            return nullptr;
        }
    }

    int64_t FormatContext::numberStream() const {
        return int64_t(_streams.size());
    }

    AVInputFormat* FormatContext::inputFormat() const {
        return _format_context->iformat;
    }

    AVOutputFormat* FormatContext::outputFormat() const{
        return _format_context->oformat;
    }

    bool FormatContext::supportsVideo() {
        // return inited_ptr(bestStream(MediaType::MEDIA_TYPE_VIDEO)); //TODO объеденить? сорс не знает о своих потоках до открытия..
        if (is("Source")) {
            return inited_ptr(bestStream(MediaType::MEDIA_TYPE_VIDEO));
        }
        if (is("Sink")) {
            return inited_codec_id(outputFormat()->video_codec); //TODO защиту от пнг в мп3
        }
        return false;
    }

    bool FormatContext::supportsAudio(){
        if (this->is("Source")) {
            return inited_ptr(bestStream(MediaType::MEDIA_TYPE_AUDIO));
        }
        if (this->is("Sink")) {
            return inited_codec_id(outputFormat()->audio_codec);
        }
        return false;
    }

    StreamVector FormatContext::streams() const {
        return _streams;
    }

    StreamVector FormatContext::streams(MediaType media_type) {
        StreamVector streams;
        for (auto&& str : _streams) {
            if (str->typeIs(media_type)) { streams.push_back(str); }
        }
        return streams;
    }

} // namespace fpp
