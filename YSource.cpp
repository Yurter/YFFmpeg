#include "YSource.h"
#include <exception>
#include <iostream>

YSource::YSource(const std::string &mrl, YMediaPreset preset) :
    YAbstractMedia(mrl),
    _input_format(nullptr)
{
    switch (preset) {
    case Auto:
        break;
    case Virtual:
        avdevice_register_all();
        guessInputFromat();
        parseInputFormat();
        _artificial_delay = 1024; //TODO: 1024 для аудио, для видео - ?
        break;
    default:
        std::cerr << "[YSource] Invalid preset." << std::endl;
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
//        while (!running()) {
//            utils::sleep_for(10);
//        }
    }
    return _opened;
}

//bool YSource::open()
//{
//    if (_opened) { return false; }
//    _opened = openInput();
//    if (_opened) {
//        _io_thread = YThread(std::bind(&YSource::read, this));\
//        _io_thread.start();
////        while (!running()) {
////            utils::sleep_for(10);
////        }
//    }
//    return _opened;
//}

bool YSource::close()
{
    if (!_opened) { return false; }
    quit();
    if (_media_format_context != nullptr) { avformat_close_input(&_media_format_context); }
    if (!YAbstractMedia::close()) { return false; }
    std::cout << "[YSource] Source: \"" << _media_resource_locator << "\" closed." << std::endl;
    return true;
}

bool YSource::guessInputFromat()
{
    AVInputFormat* input_format = av_find_input_format(guessFormatShortName().c_str());
    if (input_format == nullptr) {
        std::cerr << "[YAbstractMedia] Failed guess input format: " << _media_resource_locator << std::endl;
        return false;
    }
    _input_format = input_format;
    return true;
}

bool YSource::openInput()
{
    std::cout << "[YSource] Source: \"" << _media_resource_locator << "\" is opening..." << std::endl;
    if (_media_resource_locator.empty()) { return false; }
    if (avformat_open_input(&_media_format_context, _media_resource_locator.c_str(), _input_format, nullptr) < 0) {
        std::cerr << "[YSource] Failed to open input context." << std::endl;
        return false;
    }
    if (avformat_find_stream_info(_media_format_context, nullptr) < 0) {
        std::cerr << "[YSource] Failed to retrieve input video stream information." << std::endl;
        return false;
    }
    {
        _input_format = _media_format_context->iformat;
        parseFormatContext();
        av_dump_format(_media_format_context, 0, _media_resource_locator.c_str(), 0);
        std::cout << "[YSource] Source: \"" << _media_resource_locator << "\" opened." << std::endl;
        return true;
    }
}

YCode YSource::read()
{
    print_func
    YPacket packet;
    if (av_read_frame(mediaFormatContext(), &packet.raw()) != 0) { //TODO parse return value
        std::cerr << "[YSource] Cannot read source: \"" << _media_resource_locator << "\". Error or EOF." << std::endl;
        return YCode::ERR;
    }
    push(packet);

//    std::cout << "size " << size() << std::endl;

    return YCode::OK;
}

YCode YSource::processInputData(YPacket& input_data)
{
    return YCode::OK;
//    print_func
    if (input_data.raw().stream_index == video_parameters.streamIndex()) {
        input_data.setType(YMediaType::MEDIA_TYPE_VIDEO);
    }
    if (input_data.raw().stream_index == audio_parameters.streamIndex()) {
        input_data.setType(YMediaType::MEDIA_TYPE_AUDIO);
    }
    if (input_data.type() == YMediaType::MEDIA_TYPE_UNKNOWN) {
        return YCode::INVALID_INPUT;
    }
    if (input_data.isVideo() && video_parameters.ignore()) { return YCode::AGAIN; }
    if (input_data.isAudio() && audio_parameters.ignore()) { return YCode::AGAIN; }
    sendOutputData(input_data);
    if (_artificial_delay > 0) { utils::sleep_for(_artificial_delay); } //TODO
    return YCode::OK;
}

void YSource::parseInputFormat()
{
    // ?
}
