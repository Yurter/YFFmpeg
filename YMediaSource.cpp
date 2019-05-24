#include "YMediaSource.h"
#include <exception>
#include <iostream>
#include <thread>

YMediaSource::YMediaSource(const std::string &mrl) :
    YAbstractMedia(mrl),
    _input_format(nullptr)
{
    //
}


YMediaSource::~YMediaSource()
{
	close();
}

bool YMediaSource::open()
{
    if (_is_opened) {
        std::cerr << "[YMediaSource] Already opened." << std::endl;
        return false;
    }
    _is_opened = openInput();
    if (_is_opened) { run(); }
    return _is_opened;
}

bool YMediaSource::close()
{
    if (!YAbstractMedia::close()) { return false; }
    avformat_close_input(&_media_format_context);
    std::cout << "[YMediaSource] Source: \"" << _media_resource_locator << "\" closed." << std::endl;
    return true;
}

bool YMediaSource::readPacket(AVPacket &packet)
{
    std::lock_guard<std::mutex> lock(_packet_queue_mutex);
    if (_packet_queue.empty()) { return false; }
    packet = _packet_queue.front();
    _packet_queue.pop();
    return true;
}

bool YMediaSource::openInput()
{
    if (_media_resource_locator.empty()) {
        std::cerr << "[YMediaSource] Media resource locator is empty. Cannot start read." << std::endl;
        return false;
    }
    std::cout << "[YMediaSource] Source: \"" << _media_resource_locator << "\" is opening..." << std::endl;
    if (avformat_open_input(&_media_format_context, _media_resource_locator.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "[YMediaSource] Failed to open input context." << std::endl;
        return false;
    }
    if (avformat_find_stream_info(_media_format_context, nullptr) < 0) {
        std::cerr << "[YMediaSource] Failed to retrieve input video stream information." << std::endl;
        return false;
    }
    {
        parseFormatContext();
        av_dump_format(_media_format_context, 0, _media_resource_locator.c_str(), 0);
        std::cout << "[YMediaSource] Source: \"" << _media_resource_locator << "\" opened." << std::endl;
        return true;
    }
}

void YMediaSource::run()
{
    _thread = std::thread([this](){
        while (_is_opened) {
            AVPacket packet;
            _is_active = (av_read_frame(_media_format_context, &packet) == 0);
            if (!_is_active) {
                std::cerr << "[YMediaSource] Cannot read source: \"" << _media_resource_locator << "\". Error or EOF." << std::endl;
                close();
                return;
            } else {
                queuePacket(packet);
            }
        }
    });
}
