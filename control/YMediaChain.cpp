#include "YMediaChain.h"
#include <algorithm>

YMediaChain::YMediaChain()
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

bool YMediaChain::init()
{
    log_info("Initialization started...");

    determineSequence();


    initProcesors();
    openProcesors();
    connectProcessors();
    startProcesors();

    if (rescalerRequired()) {
        _rescaler = new YRescaler();
        if (!_rescaler->init(_decoder->videoCodecContext()
                              , _encoder->videoCodecContext())) { return false; }
    }

    if (resamplerRequired()) {
        _resampler = new YResampler();
        if (!_resampler->init(_decoder->audioCodecContext()
                              , _encoder->audioCodecContext())) { return false; }
        _resampler->setIgnoreType(YMediaType::MEDIA_TYPE_VIDEO); //TODO
    }



    //TODO
    YStream* inVidStr = _source->stream(_source->video_parameters.streamIndex());
    YStream* inAudStr = _source->stream(_source->audio_parameters.streamIndex());
    YStream* ouVidStr = _destination->stream(_destination->video_parameters.streamIndex());
    YStream* ouAudStr = _destination->stream(_destination->audio_parameters.streamIndex());

    if (!_stream_map->addRoute(inVidStr, ouVidStr)) {
        int debug_stop = 0;
    }
    if (!_stream_map->addRoute(inAudStr, ouAudStr)) {
        int debug_stop = 1;
    }

    if (optionInstalled(COPY_VIDEO)) {
        _decoder->setSkipType(YMediaType::MEDIA_TYPE_VIDEO);
        _rescaler->setSkipType(YMediaType::MEDIA_TYPE_VIDEO);
//        _video_filter->setSkipType(YMediaType::MEDIA_TYPE_VIDEO);
    }

    if (optionInstalled(COPY_AUDIO)) {
        _decoder->setSkipType(YMediaType::MEDIA_TYPE_AUDIO);
        _resampler->setSkipType(YMediaType::MEDIA_TYPE_AUDIO);
//        _audio_filter->setSkipType(YMediaType::MEDIA_TYPE_AUDIO);
    }

    _source->connectOutputTo(_decoder);
    _decoder->connectOutputTo(_resampler);
    _resampler->connectOutputTo(_encoder);
    _encoder->connectOutputTo(_stream_map);
    _stream_map->connectOutputTo(_destination);

    _source->start();
    _decoder->start();
    _resampler->start();
//    _resampler->setIgnoreType(YMediaType::MEDIA_TYPE_VIDEO); //TODO!!!
    _encoder->start();
    _stream_map->start();
    _destination->start();

//    while (true) {
//        //
//    }

    _inited = true;
    return true;
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

bool YMediaChain::rescalerRequired()
{
    return true;
//    return false;
}

bool YMediaChain::resamplerRequired()
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

YCode YMediaChain::determineSequence(YStream* src_stream, YStream* dst_stream)
{
    std::list<YObject*> sequence;
    _processor_sequences.push_back();
}

YCode YMediaChain::initProcesors()
{
    for (auto&& codec : _data_processors_codec) { codec->init(); }
    for (auto&& refi : _data_processors_refi) { refi->init(); }
}

YCode YMediaChain::connectProcessors()
{
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
