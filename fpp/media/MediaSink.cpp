#include "MediaSink.hpp"
//#include <exception>

namespace fpp {

    MediaSink::MediaSink(const std::string& mrl, Preset preset)
        : _output_format_context { mrl, preset } {
        setName("MediaSink");
        setStreams(_output_format_context.streams());
    }

    MediaSink::~MediaSink() {
        try_throw(close());
    }

    void MediaSink::init() {
        log_debug("Initialization");
        setInited(true);
    }

    Code MediaSink::open() {
        return_if(opened(), Code::OK);
        return_if_not(inited(), Code::NOT_INITED);
        log_debug("Opening");
        log_info("'" << _output_format_context.mediaResourceLocator() << "' is opening...");
        _output_format_context.open();
        log_info("'" <<_output_format_context.mediaResourceLocator() << "' opened");
        // НЕ УДАЛЯТЬ
//        _flush_timer.setInterval([&](){
//            Code ret = _output_format_context.flush();
//            if (utils::exit_code(ret)) {
//                return;
//            }
//            log_debug("Flushed");
//        }, 5 * 60'000);
        // ~ НЕ УДАЛЯТЬ
        setOpened(true);
        return Code::OK;
    }

    Code MediaSink::close() {
        return_if(closed(), Code::OK);
        log_debug("Closing");
        stopWait();
        try_to(stop());
        _output_format_context.close();
        log_info("Destination '" << _output_format_context.mediaResourceLocator() << "' closed, " //TODO метод отрабатывает дважды: из деструктора и из онСтоп
                 << utils::time_to_string(stream(0)->params->duration(), stream(0)->params->timeBase()));
        for (auto stream : streams()) { //Debug log
            log_warning("stream suration: " << utils::time_to_string(stream->params->duration(), stream->params->timeBase()));
        }
        if (stream(0)->params->duration() == 0) { //TODO кривой код
            log_warning("'" << _output_format_context.mediaResourceLocator() << "' closed empty!");
        }
        setOpened(false);
        return Code::OK;
    }

    std::string MediaSink::toString() const {
        return _output_format_context.toString();
    }

    bool MediaSink::equalTo(const SharedProcessor other) const {
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
//        auto data_stream = stream(input_data.streamIndex());
//        try_to(data_stream->stampPacket(input_data));
        try_to(storeOutputData(input_data));
        return Code::OK;
    }

    Code MediaSink::writeOutputData(Packet output_data) {
        (_output_format_context.write(output_data));
        return Code::OK;
    }

} // namespace fpp
