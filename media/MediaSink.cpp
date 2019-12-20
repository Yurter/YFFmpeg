#include "MediaSink.hpp"
#include <exception>

namespace fpp {

    MediaSink::MediaSink(const std::string mrl, IOType preset) :
        _output_format_context(mrl, /*this,*/ preset)
    {
        setName("MediaSink");
    }

    MediaSink::~MediaSink() {
        join();
        try_throw(close());
    }

    Code MediaSink::init() {
        return_if(inited(), Code::INVALID_CALL_ORDER);
        log_debug("Initialization");
        try_to(_output_format_context.init());
        try_to(setStreams(_output_format_context.streams()));
        setInited(true);
        return Code::OK;
    }

    Code MediaSink::open() {
        return_if(opened(), Code::OK);
        return_if_not(inited(), Code::NOT_INITED);
        log_debug("Opening");
        log_info(_output_format_context.mediaResourceLocator() << "\" is opening...");
        try_to(_output_format_context.open());
        log_info(_output_format_context.mediaResourceLocator() << "\" opened");
        setOpened(true);
        return Code::OK;
    }

    Code MediaSink::close() {
        return_if(closed(), Code::OK);
        log_debug("Closing");
        try_to(stop());
        if (buferIsEmpty()) {
            stopWait(); //TODO костыль?
        }
        log_info("Destination: \"" << _output_format_context.mediaResourceLocator() << "\" closed");
        if (_output_format_context.stream(0)->packetIndex() == 0) {
            log_warning(_output_format_context.mediaResourceLocator() << " closed empty!");
        }
        log_error("Sink dur = " << stream(0)->params->duration() << "ms | "
                  << stream(0)->params->duration() / 1000 << "s | "
                  << stream(0)->params->duration() / (60 * 1000) << "min | ");
        log_error("write = " << stream(0)->packetIndex());
        setOpened(false);
        return Code::OK;
    }

    std::string MediaSink::toString() const {
        return _output_format_context.toString();
    }

    bool MediaSink::equalTo(const ProcessorPointer other) const {
        auto other_media_sink = dynamic_cast<const MediaSink * const>(other.get());
        return_if(not_inited_ptr(other_media_sink), false);

        return_if(this->outputFormatContext()->mediaResourceLocator()
                  == other_media_sink->outputFormatContext()->mediaResourceLocator(), true);

        return false;
    }

    const OutputFormatContext* MediaSink::outputFormatContext() const {
        return &_output_format_context;
    }

    Code MediaSink::processInputData(Packet input_data) {
        if (input_data.isVideo()) { //Debug if
            try_to(_output_format_context.stream(input_data.streamIndex())->stampPacket(input_data));
            if (_output_format_context.mediaResourceLocator() == std::string("group_video/event.flv")) {
//                log_warning("packetIndex = " << _output_format_context.stream(input_data.streamIndex())->packetIndex());
            }
        }
//        log_debug(input_data);
//        log_debug(input_data.pts() << " " << input_data.dts() << " " << input_data.duration() << " | "
//                  << utils::rational_to_string(stream(0)->params->timeBase()) << " "
//                  << utils::rational_to_string(stream(0)->raw()->time_base));
        try_to(storeOutputData(input_data));
        return Code::OK;
    }

    Code MediaSink::writeOutputData(Packet output_data) {
//        if (av_interleaved_write_frame(_format_context, &packet.raw()) < 0) {
        log_debug("WRITE PACKET PTS: " << output_data.pts() << " " << output_data.dts());
        if (av_write_frame(_output_format_context.mediaFormatContext(), &output_data.raw()) < 0) {
            log_error("opened: " << utils::bool_to_string(opened()));
            log_error("running: " << utils::bool_to_string(running()));
            log_error("packet: " << output_data);
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
