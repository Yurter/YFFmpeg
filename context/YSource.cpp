#include "YSource.h"

YSource::YSource(const std::string& mrl, YMediaPreset preset) :
    YAbstractMedia(mrl),
    _input_format(nullptr)
{
    setName("YSource");
    switch (preset) {
    case Auto:
        break;
    case Virtual:
        avdevice_register_all();
        guessInputFromat();
        parseInputFormat();
        _artificial_delay = 1024; //TODO: 1024 для аудио, для видео - ? (1000 / frame_rate)
        break;
    default:
        log_error("Invalid preset");
        break;
    }
}

YSource::~YSource()
{
	close();
}

YCode YSource::open()
{
    return_if(opened(), YCode::INVALID_CALL_ORDER);
    try_to(openContext());
    _io_thread = YThread(std::bind(&YSource::read, this));
    _io_thread.start();
    return YCode::OK;
}

YCode YSource::close()//TODO
{
    return_if(closed(), YCode::INVALID_CALL_ORDER);
    _io_thread.quit(); //TODO
    quit();
    if (_media_format_context != nullptr) { avformat_close_input(&_media_format_context); }
    return_if_not(YAbstractMedia::close(), YCode::ERR);
    log_info("Source: \"" << _media_resource_locator << "\" closed.");
    return YCode::OK;
}

YCode YSource::guessInputFromat()
{
    AVInputFormat* input_format = av_find_input_format(guessFormatShortName().c_str());
    if (input_format == nullptr) {
        log_error("Failed guess input format: " << _media_resource_locator);
        return YCode::INVALID_INPUT;
    }
    _input_format = input_format;
    return YCode::OK;
}

YCode YSource::openContext()
{
    log_info("Source: \"" << _media_resource_locator << "\" is opening...");
    return_if(_media_resource_locator.empty(), YCode::INVALID_INPUT);
    if (avformat_open_input(&_media_format_context, _media_resource_locator.c_str(), _input_format, nullptr) < 0) {
        log_error("Failed to open input context.");
        return YCode::INVALID_INPUT;
    }
    if (avformat_find_stream_info(_media_format_context, nullptr) < 0) {
        log_error("Failed to retrieve input video stream information.");
        return YCode::ERR;
    }
    {
        _input_format = _media_format_context->iformat;
        parseFormatContext();
        av_dump_format(_media_format_context, 0, _media_resource_locator.c_str(), 0);
        log_info("Source: \"" << _media_resource_locator << "\" opened.");
        return YCode::OK;
    }
}

YCode YSource::read()
{
    YPacket packet;
    if (av_read_frame(mediaFormatContext(), &packet.raw()) != 0) { //TODO parse return value
        log_error("Cannot read source: \"" << _media_resource_locator << "\". Error or EOF.");
        return YCode::ERR;
    }
    push(packet);
    return YCode::OK;
}

YCode YSource::processInputData(YPacket& input_data)
{
    auto packet_stream = stream(input_data.raw().stream_index);
    return_if(packet_stream == nullptr, YCode::INVALID_INPUT);
    input_data.setType(packet_stream->type());
    input_data.setStreamUid(packet_stream->uid());
    if (inited_int(_artificial_delay)) { utils::sleep_for(_artificial_delay); }
    return sendOutputData(input_data);
}

void YSource::parseInputFormat()
{
    // ? TODO
}
