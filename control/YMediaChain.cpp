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

void YMediaChain::addSource(const std::string& mrl, YMediaPreset preset)
{
    _data_processors.push_back(new YSource(mrl, preset));
}

void YMediaChain::addDestination(const std::string &mrl, YMediaPreset preset)
{
    _data_processors.push_back(new YDestination(mrl, preset));
}

bool YMediaChain::init()
{
    log_info("Initialization started...");

    createProcessors();
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
            return proc->las
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
    std::for_each(_data_processors.begin()
                  , _data_processors.end()
                  , [](YThread* proc){ proc->start(); });
}

YCode YMediaChain::stopProcesors()
{
    std::for_each(_data_processors.begin()
                  , _data_processors.end()
                  , [](YThread* proc){ proc->quit(); });
}

YCode YMediaChain::openProcesors()
{
    std::for_each(_data_processors.begin()
                  , _data_processors.end()
                  , [](YThread* proc){
        YSource* ptrs = dynamic_cast<YSource*>(proc);
        if (ptrs != nullptr) { ptrs->open(); }
        YDestination* ptrd = dynamic_cast<YDestination*>(proc);
        if (ptrd != nullptr) { ptrd->open(); }
    });
}

YCode YMediaChain::closeProcesors()
{
    std::for_each(_data_processors.begin()
                  , _data_processors.end()
                  , [](YThread* proc){
        YSource* ptrs = dynamic_cast<YSource*>(proc);
        if (ptrs != nullptr) { ptrs->close(); }
        YDestination* ptrd = dynamic_cast<YDestination*>(proc);
        if (ptrd != nullptr) { ptrd->close(); }
    });
}

YCode YMediaChain::createProcessors()
{
    //
}

YCode YMediaChain::initProcesors()
{
    std::for_each(_data_processors.begin()
                  , _data_processors.end()
                  , [](YThread* proc){
        YAbstractFrameProcessor* ptrs = dynamic_cast<YAbstractFrameProcessor*>(proc);
        if (ptrs != nullptr) { ptrs->init(); }
        YDestination* ptrd = dynamic_cast<YDestination*>(proc);
        if (ptrd != nullptr) { ptrd->init(); }
    });
}

YCode YMediaChain::connectProcessors()
{
    return YCode::OK;
}

void YMediaChain::freeProcesors() //TODO
{
    std::for_each(_data_processors.begin()
                  , _data_processors.end()
                  , [](YThread* proc){ delete proc; });
}
