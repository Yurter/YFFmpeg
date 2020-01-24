#include "FormatContext.hpp"
#include <core/Utils.hpp>

namespace ffmpeg { extern "C" {
    #include <libavformat/avformat.h>
    #include <libavdevice/avdevice.h>
} } // namespace ffmpeg

namespace fpp {

    FormatContext::FormatContext(const std::string& mrl, Preset preset)
        : _media_resource_locator { mrl }
        , _preset { preset }
        , _opened { false }
        , _current_interrupter { Interrupter { InterruptedProcess::None } } {
        setName("FormatContext");
    }

    FormatContext::~FormatContext() {
        try_throw(close());
    }

    Code FormatContext::close() {
        return_if(closed(), Code::OK);
        try_to(closeContext());
        setOpened(false);
        return Code::OK;
    }

    bool FormatContext::opened() const {
        return _opened;
    }

    bool FormatContext::closed() const {
        return !_opened;
    }

    std::string FormatContext::toString() const {
        std::string context_info = "mrl: '" + mediaResourceLocator() +  "',";
        for (const auto& stream : streams()) {
            context_info += '\n' + stream->toString();
        }
        return context_info;
    }

    void FormatContext::setFormatContext(SharedAVFormatContext format_context) {
        _format_context = format_context;
    }

    void FormatContext::setOpened(bool opened) {
        _opened = opened;
    }

    Preset FormatContext::preset() const {
        return _preset;
    }

    bool FormatContext::presetIs(Preset value) const {
        return _preset == value;
    }

    Code FormatContext::open() {
        return_if(opened(), Code::OK);
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
        _format_context->interrupt_callback.opaque   = nullptr; /* Бессмысленно :( */
    }

    int FormatContext::interrupt_callback(void* opaque) {
        auto interrupter = reinterpret_cast<Interrupter*>(opaque);
        switch (interrupter->interrupted_process) {
        case InterruptedProcess::None:
            return 0;
        case InterruptedProcess::Opening: {
            const int64_t opening_timeout_ms = 10'000;
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
    }

    std::string FormatContext::mediaResourceLocator() const {
        return _media_resource_locator;
    }

    SharedAVFormatContext FormatContext::context() const {
        return _format_context;
    }

    void FormatContext::setStreams(StreamVector stream_list) {
        _streams = stream_list;
    }

    int64_t FormatContext::streamAmount() const {
        return int64_t(_format_context->nb_streams);
    }

    StreamVector FormatContext::streams() {
        return _streams;
    }

    const StreamVector FormatContext::streams() const {
        return _streams;
    }

    SharedStream FormatContext::stream(int64_t index) {
       return _streams.at(size_t(index));
    }

} // namespace fpp
