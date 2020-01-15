#include "FormatContext.hpp"
#include "core/utils.hpp"

namespace fpp {

    FormatContext::FormatContext(const std::string& mrl, StreamSetter stream_setter, IOPreset preset)
        : _format_context { nullptr }
        , _media_resource_locator { mrl }
        , _preset { preset }
        , _opened { false }
        , _artificial_delay { 0 }
        , _current_interrupter { Interrupter { InterruptedProcess::None, this } }
        , _stream_setter { stream_setter } {
        setName("FormatContext");
    }

    FormatContext::~FormatContext() {
        try_throw(close());
    }

    Code FormatContext::close() {
        return_if(closed(), Code::OK);
        try_to(closeContext());
        avformat_free_context(_format_context);
        setOpened(false);
        return Code::OK;
    }

    bool FormatContext::opened() const {
        return _opened;
    }

    bool FormatContext::closed() const {
        return !_opened;
    }

    void FormatContext::setOpened(bool opened) {
        _opened = opened;
    }

    IOPreset FormatContext::preset() const {
        return _preset;
    }

    bool FormatContext::presetIs(IOPreset value) const {
        return _preset == value;
    }

    Code FormatContext::open() {
        return_if(opened(), Code::OK);
        if_not(inited()) { try_to(init()); } //TODO оставить изменить?
        setInteruptCallback(InterruptedProcess::Opening);
        try_to(openContext());
        resetInteruptCallback();
        setOpened(true);
        return Code::OK;
    }

    void FormatContext::setInteruptCallback(InterruptedProcess process) {
        _current_interrupter.interrupted_process = process;
        _current_interrupter.chronometer.reset_timepoint();
        _format_context->interrupt_callback.callback =
                &FormatContext::interrupt_callback;
        _format_context->interrupt_callback.opaque =
                &_current_interrupter;
    }

    void FormatContext::resetInteruptCallback() {
        _current_interrupter.interrupted_process = InterruptedProcess::None;
        _format_context->interrupt_callback.callback = nullptr; /* Бессмысленно :( */
        _format_context->interrupt_callback.opaque = nullptr;   /* Бессмысленно :( */
    }

    int FormatContext::interrupt_callback(void* opaque) {
        auto interrupter = reinterpret_cast<Interrupter*>(opaque);
        switch (interrupter->interrupted_process) {
        case InterruptedProcess::None:
            return 0;
        case InterruptedProcess::Opening: {
            int opening_timeout_ms = 5000; //TODO вынести константу
            if (interrupter->chronometer.elapsed_milliseconds() > opening_timeout_ms) {
                static_log_error("interrupt_callback", "Opening timed out: " << opening_timeout_ms);
                return 1;
            }
            return 0;
        }
        case InterruptedProcess::Closing:
            static_log_error("interrupt_callback", "InterruptedProcess::Closing is not implemeted");
            return -1;
        case InterruptedProcess::Reading:
            static_log_error("interrupt_callback", "InterruptedProcess::Reading is not implemeted");
            return -1;
        case InterruptedProcess::Writing:
            static_log_error("interrupt_callback", "InterruptedProcess::Writing is not implemeted");
            return -1;
        }
        static_log_warning("interrupt_callback", "opaque = " << opaque);
        return 0;
    }

    std::string FormatContext::mediaResourceLocator() const {
        return _media_resource_locator;
    }

    AVFormatContext* FormatContext::mediaFormatContext() const {
        return _format_context;
    }

    AVFormatContext** FormatContext::mediaFormatContext2() {
        return &_format_context;
    }

    AVStream* FormatContext::stream(int64_t index) {
       return  _format_context->streams[size_t(index)];
    }

    int64_t FormatContext::numberStream() const {
        return int64_t(_format_context->nb_streams);
    }

    AVInputFormat* FormatContext::inputFormat() const {
        return _format_context->iformat;
    }

    AVOutputFormat* FormatContext::outputFormat() const{
        return _format_context->oformat;
    }

    Code FormatContext::registerStreams(StreamVector stream_list) {
        return _stream_setter(stream_list);
    }

    Code FormatContext::setMediaFormatContext(AVFormatContext* value) {
        return_if(not_inited_ptr(value), Code::INVALID_INPUT);
        _format_context = value;
        return Code::OK;
    }

} // namespace fpp
