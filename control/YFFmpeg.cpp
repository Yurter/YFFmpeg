#include "YFFmpeg.h"
#include <algorithm>


YFFmpeg::YFFmpeg() :
    _stream_map(new YStreamMap)
{
    setName("YFFmpeg");
}

YFFmpeg::~YFFmpeg()
{
    freeProcesors();
    stop();
}

bool YFFmpeg::stop()
{
    stopProcesors();
    log_info("Stopped");
    return true;
}

void YFFmpeg::pause()
{
    _paused = true;
    log_info("Paused");
}

void YFFmpeg::unpause()
{
    log_info("Unpaused");
    _paused = false;
}

void YFFmpeg::setOptions(int64_t options)
{
    _options = options;
}

void YFFmpeg::addElement(YObject* element)
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
    auto refi = dynamic_cast<YAbstractRefi*>(element); //TODO игнорируется
    if (refi != nullptr) {
        _data_processors_refi.push_back(refi);
        return;
    }
}

void YFFmpeg::setRoute(stream_context source, stream_context destination)
{
    _stream_map->addRoute(source, destination);
}

YCode YFFmpeg::init()
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

YCode YFFmpeg::run()
{
    if (!inited()) {
        try_to(init());
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

bool YFFmpeg::rescalerRequired(streams_pair streams) //TODO
{
    return_if(streams.first->isAudio(), false);
    return_if(streams.second->isAudio(), false);
    return false; //TODO
}

bool YFFmpeg::resamplerRequired(streams_pair streams)
{
    return_if(streams.first->isVideo(), false);
    return_if(streams.second->isVideo(), false);

    YAudioStream* in_stream = dynamic_cast<YAudioStream*>(streams.first);
    YAudioStream* out_stream = dynamic_cast<YAudioStream*>(streams.second);
    YAudioParameters* in = &in_stream->parameters;
    YAudioParameters* out = &out_stream->parameters;

    return_if(in->sampleRate()      != out->sampleRate(),       true);
    return_if(in->sampleFormat()    != out->sampleFormat(),     true);
    return_if(in->channels()        != out->channels(),         true);
    return_if(in->chanelsLayout()   != out->chanelsLayout(),    true);

    return false;
}

bool YFFmpeg::contingencyVideoSourceRequired() //TODO
{
    return false;
}

bool YFFmpeg::contingencyAudioSourceRequired() //TODO
{
//    return !_source->audio_parameters.available()
//            || _source->audio_parameters.ignore();
    return false;
}

bool YFFmpeg::option(YOption option) const
{
    return _options & option;
}

//void YFFmpeg::parseOptions()
//{
////    if (option(COPY_VIDEO)) {
////        _destination->video_parameters = _source->video_parameters;
////    }
////    if (option(COPY_AUDIO)) {
////        _destination->audio_parameters = _source->audio_parameters;
////    }
//}

void YFFmpeg::completeDestinationParametres()
{
//    _destination->video_parameters.softCopy(_source->video_parameters);
//    _destination->audio_parameters.softCopy(_source->audio_parameters);
}

bool YFFmpeg::inited() const
{
    return _inited;
}

void YFFmpeg::setInited(bool inited)
{
    _inited = inited;
}

YCode YFFmpeg::initRefi()
{
    for (auto&& refi : _data_processors_refi) {
        try_to(refi->init());
    }
    return YCode::OK;
}

YCode YFFmpeg::initCodec()
{
    for (auto&& refi : _data_processors_codec) {
        try_to(refi->init());
    }
    return YCode::OK;
}

YCode YFFmpeg::openContext()
{
    for (auto&& context : _data_processors_context) {
        try_to(context->open());
    }
    return YCode::OK;
}

YCode YFFmpeg::closeContext()
{
    for (auto&& context : _data_processors_context) {
        try_to(context->close());
    }
    return YCode::OK;
}

YCode YFFmpeg::startProcesors()
{
    for (auto&& context : _data_processors_context) { context->start(); }
    for (auto&& codec : _data_processors_codec) { codec->start(); }
    for (auto&& refi : _data_processors_refi) { refi->start(); }
    return YCode::OK;
}

YCode YFFmpeg::stopProcesors()
{
    for (auto&& context : _data_processors_context) { context->quit(); }
    return YCode::OK;
}

YCode YFFmpeg::determineSequences() //TODO
{
    for (auto&& route : _stream_map->map()) {
        YAbstractMedia* input_context = route.first.first;
        YAbstractMedia* output_context = route.second.first;
        YStream* input_stream = input_context->stream(route.first.second);
        YStream* output_stream = output_context->stream(route.second.second);
        return_if_not(input_stream->type() == output_stream->type(), YCode::INVALID_INPUT);
        return_if(input_context == output_context, YCode::INVALID_INPUT);
        std::list<YObject*> sequence;
        sequence.push_back(input_context);
        sequence.push_back(_stream_map);
        input_context->connectOutputTo(_stream_map);
        auto io_streams = streams_pair(input_stream, output_stream);
        if (input_stream->isVideo() && !option(YOption::COPY_VIDEO) && rescalerRequired(io_streams)) {
            //
            auto video_decoder = new YDecoder(input_stream);
            sequence.push_back(video_decoder);
            _data_processors_codec.push_back(video_decoder);
            //
            auto rescaler = new YRescaler(io_streams);
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
        if (input_stream->isAudio() && !option(YOption::COPY_AUDIO) && resamplerRequired(io_streams)) {
            //
            auto audio_decoder = new YDecoder(input_stream);
            sequence.push_back(audio_decoder);
            _data_processors_codec.push_back(audio_decoder);
            //
            auto resampler = new YResampler(io_streams);
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
    return_if_not(_stream_map->inited(), YCode::NOT_INITED);
    return YCode::OK;
}

YCode YFFmpeg::checkProcessors()
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

void YFFmpeg::freeProcesors() //TODO объекты в смартпоинтеры
{
    for (auto&& context : _data_processors_context) { delete context; }
    for (auto&& codec : _data_processors_codec) { delete codec; }
    for (auto&& refi : _data_processors_refi) { delete refi; }
}
