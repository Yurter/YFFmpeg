#include "YSource.h"
#include <exception>
#include <iostream>

YMediaSource::YMediaSource(const std::string &mrl, YMediaPreset preset) :
    YAbstractMedia(mrl),
    _input_format(nullptr)
{
    switch (preset) {
    case Virtual:
        avdevice_register_all();
        guessInputFromat();
        parseInputFormat();
        _artificial_delay = 1024; //TODO: 1024 для аудио, для видео - ?
        break;
    default:
        std::cerr << "[YMediaSource] Invalid preset." << std::endl;
        break;
    }
}


YMediaSource::~YMediaSource()
{
	close();
}

bool YMediaSource::open()
{
    if (_opened) { return false; }
    _opened = openInput();
    _io_thread = [](){
        return YCode::OK;
    };
    return _opened;
}

bool YMediaSource::close()
{
    if (!_opened) { return false; }
    quit();
    if (_media_format_context != nullptr) { avformat_close_input(&_media_format_context); }
    if (!YAbstractMedia::close()) { return false; }
    std::cout << "[YMediaSource] Source: \"" << _media_resource_locator << "\" closed." << std::endl;
    return true;
}

bool YMediaSource::guessInputFromat()
{
    AVInputFormat* input_format = av_find_input_format(guessFormatShortName().c_str());
    if (input_format == nullptr) {
        std::cerr << "[YAbstractMedia] Failed guess input format: " << _media_resource_locator << std::endl;
        return false;
    }
    _input_format = input_format;
    return true;
}

bool YMediaSource::openInput()
{
    std::cout << "[YMediaSource] Source: \"" << _media_resource_locator << "\" is opening..." << std::endl;
    if (_media_resource_locator.empty()) { return false; }
    if (avformat_open_input(&_media_format_context, _media_resource_locator.c_str(), _input_format, nullptr) < 0) {
        std::cerr << "[YMediaSource] Failed to open input context." << std::endl;
        return false;
    }
    if (avformat_find_stream_info(_media_format_context, nullptr) < 0) {
        std::cerr << "[YMediaSource] Failed to retrieve input video stream information." << std::endl;
        return false;
    }
    {
        _input_format = _media_format_context->iformat;
        parseFormatContext();
        av_dump_format(_media_format_context, 0, _media_resource_locator.c_str(), 0);
        std::cout << "[YMediaSource] Source: \"" << _media_resource_locator << "\" opened." << std::endl;
        return true;
    }
}

YCode YMediaSource::processInputData(YPacket& input_data)
{
    if (_packet_queue.full()) {
        utils::sleep_for(MEDIUM_DELAY_MS);
        return YCode::OK;
    }
    YPacket packet;
    if (av_read_frame(_media_format_context, &packet.raw()) != 0) {
        std::cerr << "[YMediaSource] Cannot read source: \"" << _media_resource_locator << "\". Error or EOF." << std::endl;
        return YCode::ERR;
    } else {
        analyzePacket(packet);
        if (packet.isVideo() && video_parameters.ignore()) { return YCode::OK; }
        if (packet.isAudio() && audio_parameters.ignore()) { return YCode::OK; }
//        std::cout << "[YMediaSource] " << packet.toString() << std::endl;
        _packet_queue.push(packet);
        if (_artificial_delay > 0) { utils::sleep_for(_artificial_delay); }
    }
    return YCode::OK;
}

void YMediaSource::parseInputFormat()
{
    // ?
}

void YMediaSource::analyzePacket(YPacket& packet)
{
    if (packet.raw().stream_index == video_parameters.streamIndex()) {
        packet.setType(YMediaType::MEDIA_TYPE_VIDEO);
        return;
    }
    if (packet.raw().stream_index == audio_parameters.streamIndex()) {
        packet.setType(YMediaType::MEDIA_TYPE_AUDIO);
        return;
    }
    packet.setType(YMediaType::MEDIA_TYPE_UNKNOWN);
}
