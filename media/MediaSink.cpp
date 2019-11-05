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
    }

    Code MediaSink::init() {
        try_to(_output_format_context.init());
        setInited(true);
        return Code::OK;
    }

    Code MediaSink::open() {
        return_if(opened(), Code::OK);
        return_if_not(inited(), Code::NOT_INITED);
        try_to(_output_format_context.open());
        setOpened(true);
        return Code::OK;
    }

    Code MediaSink::close() {
        return_if(closed(), Code::OK);
        try_to(quit());
        try_to(_output_format_context.close());
        log_info("Destination: \"" << _output_format_context.mediaFormatContext() << "\" closed.");
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
        }
        try_to(storeOutputData(input_data));
        return Code::OK;
    }

    Code MediaSink::writeOutputData(Packet output_data) {
//        if (av_interleaved_write_frame(_format_context, &packet.raw()) < 0) {
        log_warning("pts = " << output_data.pts());
//        std::cout << "pts = " << output_data.pts() << std::endl;
        if (av_write_frame(_output_format_context.mediaFormatContext(), &output_data.raw()) < 0) {
            log_error("Error muxing packet");
            return Code::ERR;
        }
        av_packet_unref(&output_data.raw()); //memfix TODO мув в деструктор пакета
        return Code::OK;
    }

    Code MediaSink::onStop() {
        try_to(_output_format_context.close()); // Check it
        return Code::OK;
    }

} // namespace fpp
