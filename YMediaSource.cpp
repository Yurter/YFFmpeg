#include "YMediaSource.h"
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
    if (_opened) { run(); }
    return _opened;
}

bool YMediaSource::close()
{
    if (!_opened) { return false; }
    stopThread();
    if (_media_format_context != nullptr) { avformat_close_input(&_media_format_context); }
    if (!YAbstractMedia::close()) { return false; }
    std::cout << "[YMediaSource] Source: \"" << _media_resource_locator << "\" closed." << std::endl;
    return true;
}

bool YMediaSource::readPacket(AVPacket &packet)
{
    return getPacket(packet);
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

void YMediaSource::run()
{
    if (_running) { return; }
    _running = true;
    _thread = std::thread([this](){
        while (_running) {
            if (_packet_queue.size() >= _packet_queue_capacity) { continue; }
            AVPacket packet;
            if (av_read_frame(_media_format_context, &packet) != 0) {
                std::cerr << "[YMediaSource] Cannot read source: \"" << _media_resource_locator << "\". Error or EOF." << std::endl;
                _running = false;
                break;
            } else {
                if (isVideoPacket(packet) && video_parameters.ignore()) { continue; }
                if (isAudioPacket(packet) && audio_parameters.ignore()) { continue; }
                queuePacket(packet);
            }
        }
    });
}
