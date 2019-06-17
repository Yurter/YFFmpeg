#include "YSource.h"
#include <exception>
#include <iostream>

YSource::YSource(const std::string &mrl, YMediaPreset preset) :
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

bool YSource::open()
{
    if (_opened) { return false; }
    _opened = openInput();
    if (_opened) {
        _io_thread = YThread(std::bind(&YSource::read, this));
        _io_thread.start();
    }
    return _opened;
}

bool YSource::close()
{
    if (!_opened) { return false; }
    _io_thread.quit();
    quit();
    if (_media_format_context != nullptr) { avformat_close_input(&_media_format_context); }
    if (!YAbstractMedia::close()) { return false; }
    log_info("Source: \"" << _media_resource_locator << "\" closed.");
    return true;
}

bool YSource::guessInputFromat()
{
    AVInputFormat* input_format = av_find_input_format(guessFormatShortName().c_str());
    if (input_format == nullptr) {
        log_error("Failed guess input format: " << _media_resource_locator);
        return false;
    }
    _input_format = input_format;
    return true;
}

bool YSource::openInput()
{
    log_info("Source: \"" << _media_resource_locator << "\" is opening...");
    if (_media_resource_locator.empty()) { return false; }
    if (avformat_open_input(&_media_format_context, _media_resource_locator.c_str(), _input_format, nullptr) < 0) {
        log_error("Failed to open input context.");
        return false;
    }
    if (avformat_find_stream_info(_media_format_context, nullptr) < 0) {
        log_error("Failed to retrieve input video stream information.");
        return false;
    }
    {
        _input_format = _media_format_context->iformat;
        parseFormatContext();
        av_dump_format(_media_format_context, 0, _media_resource_locator.c_str(), 0);
        log_info("Source: \"" << _media_resource_locator << "\" opened.");
        return true;
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
    if (input_data.raw().stream_index == video_parameters.streamIndex()) {
        input_data.setType(YMediaType::MEDIA_TYPE_VIDEO);
    }
    if (input_data.raw().stream_index == audio_parameters.streamIndex()) {
        input_data.setType(YMediaType::MEDIA_TYPE_AUDIO);
    }
    if (input_data.type() == YMediaType::MEDIA_TYPE_UNKNOWN) {
        return YCode::INVALID_INPUT;
    }
    sendOutputData(input_data); //TODO ret unused
    if (_artificial_delay > 0) { utils::sleep_for(_artificial_delay); } //TODO
    return YCode::OK;
}

void YSource::parseInputFormat()
{
    // ?
}
