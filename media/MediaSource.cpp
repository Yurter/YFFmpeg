#include "MediaSource.hpp"

namespace fpp {

    MediaSource::MediaSource(const std::string& mrl, IOType preset) :
        _input_format_context { mrl, preset }
    {
        setName("MediaSource");
    }

    MediaSource::~MediaSource() {
        try_throw(close());
    }

    Code MediaSource::init() {
        return_if(inited(), Code::OK);
        log_debug("Initialization");
        try_to(_input_format_context.init());
        setInited(true);
        return Code::OK;
    }

    Code MediaSource::open() {
        return_if(opened(), Code::OK);
        log_debug("Opening");
        log_info("'" << _input_format_context.mediaResourceLocator() << "' is opening...");
        try_to(_input_format_context.open());
        log_info("'" << _input_format_context.mediaResourceLocator() << "' opened");
        { //TODO refactoring
            try_to(setStreams(_input_format_context.streams()));
            for (auto&& avstream : streams()) { //TODO
                try_to(avstream->init());
            }
        }
        setOpened(true);
        return Code::OK;
    }

    Code MediaSource::close() {
        return_if(closed(), Code::OK);
        log_debug("Closing");
        try_to(sendEof()); //TODO костыль?
        try_to(stop());
        stopWait(); //TODO костыль?
        try_to(_input_format_context.close());
        log_info("Source '" << _input_format_context.mediaResourceLocator() << "' closed, "
                 << utils::time_to_string(stream(0)->params->duration(), stream(0)->params->timeBase())); //TODO индекс нулевой закардкожен - брать блиьельность из контекста, а в нем максимальную по потокам 13.01
        setOpened(false);
        return Code::OK;
    }

    std::string MediaSource::toString() const {
        /* Input #0, rtsp, from 'rtsp://admin:admin@192.168.10.3': */
        std::string str = "Input #"
//                + std::to_string(_input_format_context.uid()) + ", "
                + utils::guess_format_short_name(_input_format_context.mediaResourceLocator()) + ", "
                + "from '" + _input_format_context.mediaResourceLocator() + "'";
        return str;
    }

    bool MediaSource::equalTo(const ProcessorPointer other) const {
        auto other_media_source = dynamic_cast<const MediaSource * const>(other.get());
        return_if(not_inited_ptr(other_media_source), false);

        return_if(this->inputFormatContext()->mediaResourceLocator()
                  == other_media_source->inputFormatContext()->mediaResourceLocator(), true);

        return false;
    }

    const InputFormatContext* MediaSource::inputFormatContext() const {
        return &_input_format_context;
    }

    Code MediaSource::readInputData(Packet& input_data) {
        if (inputDataCount() == 0) { //TODO кривой костыль
            for (auto& stream : streams()) {
                try_to(_input_format_context.seek(stream->index(), stream->startTimePoint()));
            }
        }
//        try_to(_input_format_context.read(input_data));
        try_to(_input_format_context.read(input_data, _input_format_context.presetIs(IOType::Virtual) ? ReadWriteMode::Interleaved : ReadWriteMode::Instant)); //TODO refactoring 13.01
//        if (input_data.isAudio()) log_warning("1] READ: " << input_data);
        return_if(stream(input_data.streamIndex())->timeIsOver(), Code::END_OF_FILE); //TODO перенести
        return Code::OK;
    }

    Code MediaSource::processInputData(Packet input_data) {
        if (stream(input_data.streamIndex())->packetIndex() == 0) { //TODO костыль? каждый раз проверяет.. -> перенести в онСтарт - раз прочитать пакеты из каждого потока
            stream(input_data.streamIndex())->determineStampType(input_data);
        }
//        if (input_data.isAudio()) {
//            log_warning("RAW AUDIO: " << input_data);
//        }

        auto data_stream = stream(input_data.streamIndex());
        try_to(stream(input_data.streamIndex())->stampPacket(input_data/*, data_stream->params->timeBase()*/));
//        if (input_data.isAudio()) log_warning("1] READ: " << input_data);

        if (inputDataCount() == 0) { //TODO нуженл ли этот лог?
            log_debug("Read from "
                      << (stream(input_data.streamIndex())->startTimePoint() == FROM_START
                          ? "start"
                          : utils::time_to_string(stream(input_data.streamIndex())->startTimePoint(), stream(input_data.streamIndex())->params->timeBase())) //TODO кривой код
                      << ", first packet: " << input_data);
        }

        return sendOutputData(input_data, data_stream->params->streamUid());
    }

    Code MediaSource::onStop() {
        log_debug("onStop");
//        try_to(close());
        return Code::OK;
    }

} // namespace fpp
