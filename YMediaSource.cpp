#include "YMediaSource.h"
#include <exception>
#include <iostream>
#include <thread>

YMediaSource::YMediaSource(const std::string &mrl) :
	YAbstractMedia(mrl)
{
    //
}


YMediaSource::~YMediaSource()
{
	close();
}

bool YMediaSource::open()
{
	if (_media_resource_locator.empty()) {
        std::cerr << "[YMediaSource] Media resource locator is empty. Cannot start read." << std::endl;
		return false;
	}
    {
		_media_format_context = avformat_alloc_context();
		if (_media_format_context == nullptr) {
			std::cerr << "Failed to alloc input format context." << std::endl;
			return false;
		}
    }
    std::cout << "[YMediaSource] Source: \"" << _media_resource_locator << "\" is opening..." << std::endl;
    if (avformat_open_input(&_media_format_context, _media_resource_locator.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Failed to open input context." << std::endl;
        return false;
    }
    if (avformat_find_stream_info(_media_format_context, nullptr) < 0) {
        std::cerr << "[YMediaSource] Failed to retrieve input video stream information." << std::endl;
        return false;
    }
	{
        _is_opened = true;
        getInfo();
		av_dump_format(_media_format_context, 0, _media_resource_locator.c_str(), 0);
        std::cout << "[YMediaSource] Source: \"" << _media_resource_locator << "\" opened." << std::endl;
    }
	return true;
}

bool YMediaSource::close()
{
    if (!_is_opened) { return false; }
    _is_opened = false;
    std::cout << "[YMediaSource] Source: \"" << _media_resource_locator << "\" closed." << std::endl;
    return true;
}

AVPacket YMediaSource::readPacket()
{
    AVPacket packet;
    if (av_read_frame(_media_format_context, &packet) < 0) {
        std::cerr << "[YMediaSource] Cannot read source: \"" << _media_resource_locator << "\". Error or EOF." << std::endl;
        return AVPacket();
    } else {
        return packet;
    }
}
