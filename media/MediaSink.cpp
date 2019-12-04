#include "MediaSink.hpp"
#include <exception>

namespace fpp {

    MediaSink::MediaSink(const std::string mrl, IOType preset) :
        _output_format_context(mrl, this, preset)
    {
        setName("MediaSink");
    }

    MediaSink::~MediaSink() {
        try_throw(close());
//        join(); //TODO виснет на удалении объекта
    }

    Code MediaSink::init() {
        return_if(inited(), Code::INVALID_CALL_ORDER);
        log_debug("Initialization.");
        try_to(_output_format_context.init());
        try_to(setStreams(_output_format_context.streams()));
        setInited(true);
        return Code::OK;
    }

    Code MediaSink::open() {
        return_if(opened(), Code::OK);
        return_if_not(inited(), Code::NOT_INITED);
        log_debug("Opening.");
        try_to(_output_format_context.open());
        setOpened(true);
        return Code::OK;
    }

//    Code MediaSink::close() {
//        return_if(closed(), Code::OK);
//        log_debug("Closing.");
//        try_to(quit());
//        try_to(_output_format_context.close());
//        log_info("Destination: \"" << _output_format_context.mediaResourceLocator() << "\" closed.");
//        log_info("To " << _output_format_context.mediaResourceLocator() << " writed "
//                 << _output_format_context.stream(0)->packetIndex() << " packets.");
//        setOpened(false);
//        return Code::OK;
//    }
    Code MediaSink::close() {
        return_if(closed(), Code::OK);
        log_debug("Closing.");
        try_to(stop());
        stopWait(); //TODO костыль?
//        try_to(_output_format_context.close());
        log_info("Destination: \"" << _output_format_context.mediaResourceLocator() << "\" closed.");
//        log_info("To " << _output_format_context.mediaResourceLocator() << " writed "
//                 << _output_format_context.stream(0)->packetIndex() << " packets.");
        setOpened(false);
        return Code::OK;
    }

    std::string MediaSink::toString() const {
        return _output_format_context.toString();
    }

    OutputFormatContext& MediaSink::outputFormatContext() {
        return _output_format_context;
    }

    Code MediaSink::processInputData(Packet input_data) {
        if (input_data.isVideo()) { //Debug if
            try_to(_output_format_context.stream(input_data.streamIndex())->stampPacket(input_data));
            if (_output_format_context.mediaResourceLocator() == std::string("group_video/event.flv")) {
//                log_warning("packetIndex = " << _output_format_context.stream(input_data.streamIndex())->packetIndex());
            }
        }
        try_to(storeOutputData(input_data));
        return Code::OK;
    }

    Code MediaSink::writeOutputData(Packet output_data) {
//        if (av_interleaved_write_frame(_format_context, &packet.raw()) < 0) {
//        if (_output_format_context.mediaResourceLocator() == std::string("group_video/event.flv")) {
////            static int
//            log_warning("pts = " << output_data.pts());
//        }
//        std::cout << "pts = " << output_data.pts() << std::endl;
        bool test = opened();
        if (av_write_frame(_output_format_context.mediaFormatContext(), &output_data.raw()) < 0) {
            log_error("Error muxing packet");
            return Code::ERR;
        }
        av_packet_unref(&output_data.raw()); //memfix TODO мув в деструктор пакета
        return Code::OK;
    }

    Code MediaSink::onStop() {
        return_if(closed(), Code::INVALID_CALL_ORDER);
        log_debug("onStop");
        try_to(_output_format_context.close()); // Check it
        return Code::OK;
    }

} // namespace fpp
