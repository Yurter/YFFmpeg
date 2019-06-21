#include "YMediaChain.h"
#include <algorithm>

YMediaChain::YMediaChain() :
    _stream_map(new YStreamMap)
{
    setName("YMediaChain");
}

YMediaChain::~YMediaChain()
{
    freeProcesors();
    stop();
}

bool YMediaChain::stop()
{
    stopProcesors();
    log_info("Stopped");
    return true;
}

void YMediaChain::pause()
{
    _paused = true;
    log_info("Paused");
}

void YMediaChain::unpause()
{
    log_info("Unpaused");
    _paused = false;
}

void YMediaChain::setOptions(int64_t options)
{
    _options = options;
}

void YMediaChain::addElement(YObject* element)
{
    auto context = dynamic_cast<YAbstractMedia*>(element);
    if (context != nullptr) {
        _data_processors_context.push_back(context);
        return;
    }
    auto codec = dynamic_cast<YAbstractCodec*>(element); //TODO игнорируется
    if (codec != nullptr) {
        _data_processors_codec.push_back(codec);
        return;
    }
    auto refi = dynamic_cast<YAbstractFrameProcessor*>(element); //TODO игнорируется
    if (refi != nullptr) {
        _data_processors_refi.push_back(refi);
        return;
    }
}

void YMediaChain::setRoute(streams_pair streams)
{
    _stream_map->addRoute(streams);
}

YCode YMediaChain::init()
{
    log_info("Initialization started...");

    try_to(determineSequences());
    try_to(initRefi());
    try_to(initCodec());
    try_to(openContext());
    try_to(startProcesors());

    setInited(true);
    return YCode::OK;
}

YCode YMediaChain::run()
{
    if (not inited()) {
        init();
    }
    for (auto&& context : _data_processors_context) {
        return_if_not(context->running(), context->exitCode());
    }
    for (auto&& codec : _data_processors_codec) {
        return_if_not(codec->running(), codec->exitCode());
    }
    for (auto&& refi : _data_processors_refi) {
        return_if_not(refi->running(), refi->exitCode());
    }
    utils::sleep_for(LONG_DELAY_MS);
    return YCode::OK;
}

bool YMediaChain::rescalerRequired(streams_pair streams)
{
    return true;
//    return false;
}

bool YMediaChain::resamplerRequired(streams_pair streams)
{
    auto& src = _source->audio_parameters;
    auto& dst = _destination->audio_parameters;
    if (!src.available())   { return false; }
    if (!dst.available())   { return false; }
    if (src.ignore())       { return false; }
    if (option(COPY_AUDIO)) { return false; }
    if (src.sampleRate() != dst.sampleRate())       { return true; }
    if (src.sampleFormat() != dst.sampleFormat())   { return true; }
    if (src.chanels() != dst.chanels())             { return true; }
    if (src.chanelsLayout() != dst.chanelsLayout()) { return true; }
    return false;
}

bool YMediaChain::contingencyVideoSourceRequired()
{
    return false;
}

bool YMediaChain::contingencyAudioSourceRequired()
{
    return !_source->audio_parameters.available()
            || _source->audio_parameters.ignore();
}

bool YMediaChain::option(YOptions option)
{
    return _options & option;
}

void YMediaChain::parseOptions()
{
    if (option(COPY_VIDEO)) {
        _destination->video_parameters = _source->video_parameters;
    }
    if (option(COPY_AUDIO)) {
        _destination->audio_parameters = _source->audio_parameters;
    }
}

void YMediaChain::completeDestinationParametres()
{
    _destination->video_parameters.softCopy(_source->video_parameters);
    _destination->audio_parameters.softCopy(_source->audio_parameters);
}

bool YMediaChain::inited() const
{
    return _inited;
}

void YMediaChain::setInited(bool inited)
{
    _inited = inited;
}

YCode YMediaChain::initRefi()
{
    for (auto&& refi : _data_processors_refi) {
        try_to(refi->init());
    }
    return YCode::OK;
}

YCode YMediaChain::initCodec()
{
    for (auto&& refi : _data_processors_codec) {
        try_to(refi->init());
    }
    return YCode::OK;
}

YCode YMediaChain::openContext()
{
    for (auto&& context : _data_processors_context) {
        try_to(context->open());
    }
    return YCode::OK;
}

YCode YMediaChain::closeContext()
{
    for (auto&& context : _data_processors_context) {
        try_to(context->close());
    }
    return YCode::OK;
}

YCode YMediaChain::startProcesors()
{
    for (auto&& context : _data_processors_context) { context->start(); }
    for (auto&& codec : _data_processors_codec) { codec->start(); }
    for (auto&& refi : _data_processors_refi) { refi->start(); }
}

YCode YMediaChain::stopProcesors()
{
    for (auto&& context : _data_processors_context) { context->quit(); }
}

YCode YMediaChain::determineSequences() //TODO
{
    for (auto&& route : _stream_map->map()) {
        YStream* input_stream = route.first;
        YStream* output_stream = route.second;
        YAbstractMedia* input_context = input_stream->mediaContext();
        YAbstractMedia* output_context = output_stream->mediaContext();
        return_if_not(input_stream->type() == output_stream->type(), YCode::INVALID_INPUT);
        return_if(input_context == output_context, YCode::INVALID_INPUT);
        std::list<YObject*> sequence;
        sequence.push_back(input_context);
        sequence.push_back(_stream_map);
        input_context->connectOutputTo(_stream_map);
        if (input_stream->isVideo() && !option(YOption::COPY_VIDEO) && rescalerRequired(route)) {
            //
            auto video_decoder = new YDecoder(input_stream);
            sequence.push_back(video_decoder);
            _data_processors_codec.push_back(video_decoder);
//            _stream_map->connectOutputTo(video_decoder);
            //
            auto rescaler = new YRescaler(route);
            sequence.push_back(rescaler);
            _data_processors_refi.push_back(rescaler);
            video_decoder->connectOutputTo(rescaler);
            //
            auto video_encoder = new YEncoder(output_stream);
            sequence.push_back(video_encoder);
            _data_processors_codec.push_back(video_encoder);
            rescaler->connectOutputTo(video_encoder);
            //
            sequence.push_back(output_context);
            video_encoder->connectOutputTo(output_context);
        }
        if (input_stream->isAudio() && !option(YOption::COPY_AUDIO) && resamplerRequired(route)) {
            //
            auto audio_decoder = new YDecoder(input_stream);
            sequence.push_back(audio_decoder);
            _data_processors_codec.push_back(audio_decoder);
//            _stream_map->connectOutputTo(audio_decoder);
            //
            auto resampler = new YResampler(route);
            sequence.push_back(resampler);
            _data_processors_refi.push_back(resampler);
            audio_decoder->connectOutputTo(resampler);
            //
            auto audio_encoder = new YEncoder(output_stream);
            sequence.push_back(audio_encoder);
            _data_processors_codec.push_back(audio_encoder);
            resampler->connectOutputTo(audio_encoder);
            //
            sequence.push_back(output_context);
            audio_encoder->connectOutputTo(output_context);
        }
        auto first_proc_it = std::next(std::find(sequence.begin(), sequence.end(), _stream_map));
        _stream_map->setRoute(input_stream, dynamic_cast<YAsyncQueue<YPacket>*>(*first_proc_it));
        _processor_sequences.push_back(sequence);
    }
}

YCode YMediaChain::defaultRelation(std::list<streams_pair>* relation_list)
{
    std::list<YSource*> source_list;
    for (auto& elem : _data_processors_context) {
        auto ptr = dynamic_cast<YSource*>(elem);
        if (ptr != nullptr) {
            source_list.push_back(ptr);
        }
    }
    return_if(source_list.size() > 1, YCode::DOUBT);
    return YCode::OK;
}

YCode YMediaChain::checkProcessors()
{
    for (auto&& context : _data_processors_context) {
        return_if_not(context->running(), context->exitCode());
    }
    for (auto&& codec : _data_processors_codec) {
        return_if_not(codec->running(), codec->exitCode());
    }
    for (auto&& refi : _data_processors_refi) {
        return_if_not(refi->running(), refi->exitCode());
    }
    return YCode::OK;
}

void YMediaChain::freeProcesors() //TODO
{
    std::for_each(_data_processors.begin()
                  , _data_processors.end()
                  , [](YThread* proc){ delete proc; });
}
