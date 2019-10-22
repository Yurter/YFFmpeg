#include "Source.hpp"

namespace fpp {

    Source::Source(const std::string& mrl, IOType preset) :
        Context(mrl, preset)
        , _input_format(nullptr)
//        , _eof_flag(false)
    {
        setName("Source");
    }

    Source::~Source() {
        close();
    }

    Code Source::init() {
        switch (_preset) {
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
        try_to(createContext());
        setInited(true);
        return Code::OK;
    }

    Code Source::open(){
        return_if(opened(), Code::INVALID_CALL_ORDER);
        try_to(openContext());
        _io_thread = Thread(std::bind(&Source::read, this));
        _io_thread.setName("IOThread");
        _io_thread.start();
        return Code::OK;
    }

    Code Source::close() { //TODO
//        log_debug("closing " << mediaResourceLocator() << " " << closed());
        sendEofPacket();
        return_if(closed(), Code::INVALID_CALL_ORDER);
        _io_thread.quit(); //TODO
        quit();
        if (_format_context != nullptr) { avformat_close_input(&_format_context); }
        try_to(Context::close());
        return_if_not(closed(), Code::ERR);
        log_info("Source: \"" << _media_resource_locator << "\" closed.");
        return Code::OK;
    }

    std::string Source::toString() const {
        /* Input #0, rtsp, from 'rtsp://admin:admin@192.168.10.3': */
        std::string str = "Input #"
                + std::to_string(uid()) + ", "
                + utils::guess_format_short_name(_media_resource_locator) + ", "
                + "from '" + _media_resource_locator + "'";
        return str;
    }

    Code Source::guessInputFromat() //TODO зачем это?
    {
        auto shorn_name = utils::guess_format_short_name(_media_resource_locator);
        AVInputFormat* input_format = av_find_input_format(shorn_name.c_str());
        if (input_format == nullptr) {
            log_error("Failed guess input format: " << _media_resource_locator);
            return Code::INVALID_INPUT;
        }
        _input_format = input_format;
        return Code::OK;
    }

    Code Source::createContext() {
        _format_context = avformat_alloc_context();
        if (not_inited_ptr(_format_context)) {
            log_error("Failed to alloc input context.");
            return Code::ERR;
        }
        return Code::OK;
    }

    Code Source::openContext() {
        log_info("Source: \"" << _media_resource_locator << "\" is opening...");
        return_if(_media_resource_locator.empty(), Code::INVALID_INPUT);
        if (avformat_open_input(&_format_context, _media_resource_locator.c_str(), _input_format, nullptr) < 0) {
            log_error("Failed to open input context.");
            return Code::INVALID_INPUT;
        }
        if (avformat_find_stream_info(_format_context, nullptr) < 0) {
            log_error("Failed to retrieve input video stream information.");
            return Code::ERR;
        }
        {
            _input_format = _format_context->iformat;
            parseFormatContext();
            av_dump_format(_format_context, 0, _media_resource_locator.c_str(), 0);
            setInited(true);
            log_info("Source: \"" << _media_resource_locator << "\" opened.");
            setOpened(true);
            return Code::OK;
        }
    }

    Code Source::read() {
        Packet packet;
        if (int ret = av_read_frame(mediaFormatContext(), &packet.raw()); ret != 0) { //TODO parse return value
            if (ret == AVERROR_EOF) {
                log_info("Source reading completed");
                return_if_not(wait_and_push(Packet()), Code::EXIT);
                return Code::END_OF_FILE;
            }
            log_error("Cannot read source: \"" << _media_resource_locator << "\". Error " << ret);
            return Code::ERR;
        }
        return_if_not(wait_and_push(packet), Code::EXIT);
        return Code::OK;
    }

    Code Source::processInputData(Packet& input_data) {
        if (input_data.empty()) {
            return sendEofPacket();
        }
        auto packet_stream = stream(input_data.raw().stream_index);
        return_if(not_inited_ptr(packet_stream), Code::AGAIN);
        return_if_not(packet_stream->used(), Code::AGAIN);
        input_data.setType(packet_stream->type());
        input_data.setStreamUid(packet_stream->uid());
        if (inited_int(_artificial_delay)) { utils::sleep_for(_artificial_delay); } //todo
        return sendOutputData(input_data);
    }

    Code Source::sendEofPacket() {
        Packet eof_packet;
        for (auto&& stream : streams()) {
            if (stream->used()) {
                eof_packet.setType(stream->type());
                eof_packet.setStreamUid(stream->uid());
                try_to(sendOutputData(eof_packet));
                stream->setUsed(false);
            }
        }
        return Code::END_OF_FILE;
    }

    void Source::parseInputFormat() {
        // ? TODO зачем это?
    }

} // namespace fpp
