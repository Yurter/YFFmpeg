#include "YMediaSource.h"
#include <exception>
#include <iostream>

YMediaSource::YMediaSource(const std::string &mrl, YMediaPreset preset) :
    YAbstractMedia(mrl),
    _input_format(nullptr)
{
    switch (preset) {
    case Auto:
        break;
    case Silence:
        avdevice_register_all();
        _media_resource_locator = "aevalsrc=0";
        _input_format = av_find_input_format("lavfi");
        if (_input_format == nullptr) {
            std::cerr << "[YMediaSource] FFmpeg lavfi format not found" << std::endl;
            break;
        }

//        std::cout << "[DEBUG] 11111111111111111111111111111111" << std::endl;
//        if (avformat_open_input(&_media_format_context, _media_resource_locator.c_str(), _input_format, nullptr) < 0) {
//            std::cout << "Failed to open virtual input context" << std::endl;
//            return;
//        }
//        std::cout << "[DEBUG] 22222222222222222222222222222222" << std::endl;

        //
//        audio_parameters.setSampleRate(44'100);
////        audio_parameters.setSampleFormat(AV_SAMPLE_FMT_S16P);
//        audio_parameters.setSampleFormat(AV_SAMPLE_FMT_FLTP);
//        audio_parameters.setBitrate(128'000);
//        audio_parameters.setChanelsLayout(AV_CH_LAYOUT_MONO);
//        audio_parameters.setChanels(1);
//        audio_parameters.setCodec("mp3");
////        audio_parameters.setCodec(AV_CODEC_ID_PCM_MULAW);
//        audio_parameters.setAvailable(true);
        //
        audio_parameters.setSampleRate(22'050);
        audio_parameters.setSampleFormat(AV_SAMPLE_FMT_S16P);
        audio_parameters.setBitrate(128 * 1024);
        audio_parameters.setChanelsLayout(AV_CH_LAYOUT_MONO);
        audio_parameters.setChanels(1);
        audio_parameters.setCodec("mp3");
        audio_parameters.setAvailable(true);
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
    return getPacket(packet);
}

bool YMediaSource::openInput()
{
    if (_media_resource_locator.empty()) {
        std::cerr << "[YMediaSource] Media resource locator is empty. Cannot start read." << std::endl;
        return false;
    }
    _media_format_context = avformat_alloc_context();
    std::cout << "[YMediaSource] Source: \"" << _media_resource_locator << "\" is opening..." << std::endl;
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
    _thread = std::thread([this](){
        while (_is_opened) {
            if (_packet_queue.size() >= _packet_queue_capacity) { continue; }
            AVPacket packet;
            _is_active = (av_read_frame(_media_format_context, &packet) == 0);
            if (!_is_active) {
                std::cerr << "[YMediaSource] Cannot read source: \"" << _media_resource_locator << "\". Error or EOF." << std::endl;
                close();
                return;
            } else {
                if (isVideoPacket(packet) && video_parameters.ignore()) { continue; }
                if (isAudioPacket(packet) && audio_parameters.ignore()) { continue; }
                queuePacket(packet);
            }
        }
    });
}
