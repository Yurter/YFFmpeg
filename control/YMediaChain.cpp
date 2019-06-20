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
}

void YMediaChain::unpause()
{
    _paused = false;
}

void YMediaChain::setOptions(int64_t options)
{
    _options = options;
}

void YMediaChain::addElement(YObject *element)
{
    auto context = dynamic_cast<YAbstractMedia*>(element);
    if (context != nullptr) {
        _data_processors_context.push_back(context);
        return;
    }
    auto codec = dynamic_cast<YAbstractCodec*>(element);
    if (codec != nullptr) {
        _data_processors_codec.push_back(codec);
        return;
    }
    auto refi = dynamic_cast<YAbstractFrameProcessor*>(element);
    if (refi != nullptr) {
        _data_processors_refi.push_back(refi);
        return;
    }
}

void YMediaChain::addRoute(streams_pair streams)
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
    try_to(connectProcessors());
    try_to(startProcesors());

    setInited(true);
    return YCode::OK;
}

YCode YMediaChain::run()
{
    if (!_inited) { //TODO
        init();
    }

    for (auto&& proc : _data_processors) {
        if (proc->running() == false) {
            return proc->erro
        }
    }

    if (_source->running() == false)        { return YCode::ERR; } //TODO return lastError
    if (_decoder->running() == false)       { return YCode::ERR; }
    if (_resampler->running() == false)     { return YCode::ERR; }
    if (_encoder->running() == false)       { return YCode::ERR; }
    if (_stream_map->running() == false)    { return YCode::ERR; }
    if (_destination->running() == false)   { return YCode::ERR; }

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

YCode YMediaChain::openProcesors()
{
    for (auto&& context : _data_processors_context) { context->open(); }
}

YCode YMediaChain::closeProcesors()
{
    for (auto&& context : _data_processors_context) { context->close(); }
}

YCode YMediaChain::determineSequences()
{
    for (auto&& route : _stream_map->map()) {
        if (route.first->type() != route.second->type()) {
            return YCode::INVALID_INPUT;
        }
        if (route.first->mediaContext() == route.second->mediaContext()) {
            return YCode::INVALID_INPUT;
        }
        std::list<YObject*> sequence;
        if (!option(YOption::COPY_VIDEO) && rescalerRequired(route)) {
            //
            auto video_decoder = new YDecoder(route.first);
            sequence.push_back(video_decoder);
            _data_processors_codec.push_back(video_decoder);
            //
            auto rescaler = new YRescaler(route);
            sequence.push_back(rescaler);
            _data_processors_refi.push_back(rescaler);
            //
            auto video_encoder = new YEncoder(route.second);
            sequence.push_back(video_encoder);
            _data_processors_codec.push_back(video_encoder);
        }
        if (!option(YOption::COPY_AUDIO) && resamplerRequired(route)) {
            //
            auto audio_decoder = new YDecoder(route.first);
            sequence.push_back(audio_decoder);
            _data_processors_codec.push_back(audio_decoder);
            //
            auto resampler = new YResampler(route);
            sequence.push_back(resampler);
            _data_processors_refi.push_back(resampler);
            //
            auto audio_encoder = new YEncoder(route.second);
            sequence.push_back(audio_encoder);
            _data_processors_codec.push_back(audio_encoder);
        }
        sequence.push_back(route.second->mediaContext());

        _stream_map->setRoute(route.first, dynamic_cast<YAsyncQueue<YPacket>*>(*sequence.begin()));
        sequence.push_front(_stream_map);
        sequence.push_front(route.first->mediaContext());
        _processor_sequences.push_back(sequence);
    }
}

YCode YMediaChain::defaultRelation(std::list<io_stream_relation>* relation_list)
{
    std::list<YSource*> source_list;
    for (auto& elem : _data_processors_context) {
        auto ptr = dynamic_cast<YSource*>(elem);
        if (ptr != nullptr) {
            source_list.push_back(ptr);
        }
    }
    if (source_list.size() > 1) {
        return YCode::DOUBT;
    }
    //
    return YCode::OK;
}


YCode YMediaChain::connectProcessors()
{
    for (auto&& sequence : _processor_sequences) {
        auto it = sequence.begin();
        while (it != sequence.end()) {
            it->connectOutputTo(++it);
        }
    }
    return YCode::OK;
}

YCode YMediaChain::ckeckProcessors()
{
    for (auto&& context : _data_processors_context) {
        if (!context->running()) { return context->exitCode(); }
    }
    for (auto&& codec : _data_processors_codec) {
        if (!codec->running()) { return codec->exitCode(); }
    }

    for (auto&& refi : _data_processors_refi) {
        if (!refi->running()) { return refi->exitCode(); }
    }
    return YCode::OK;
}

void YMediaChain::freeProcesors() //TODO
{
    std::for_each(_data_processors.begin()
                  , _data_processors.end()
                  , [](YThread* proc){ delete proc; });
}
