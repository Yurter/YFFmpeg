#pragma once
#include <core/Object.hpp>
#include <core/time/Chronometer.hpp>
#include <streamHeader.hpp>
#include <memory>

namespace ffmpeg {
    struct AVFormatContext;
    struct AVStream;
    struct AVInputFormat;
    struct AVOutputFormat;
} // namespace ffmpeg

namespace fpp {

    /* Варианты для быстрой преднастройки */
    /* параметров медиа-контекстов        */
    enum Preset { //TODO заменить на полиморфизм, либо вовсе убрать 24.01
        Auto,
        Raw,
        /* Input */
        Virtual,
        /* Output */
        Event,
        OpenCV,
        YouTube,
        Timelapse,
    };

    using SharedAVFormatContext = std::shared_ptr<ffmpeg::AVFormatContext>;

    class FormatContext : public Object {

    public:

        FormatContext(const std::string& mrl, Preset preset);
        virtual ~FormatContext() override;

        std::string             mediaResourceLocator()  const;
        SharedAVFormatContext   context()               const;
        const StreamVector      streams()               const;
        StreamVector            streams();

        Code                open();
        Code                close();

        bool                opened() const;
        bool                closed() const;

        SharedStream        stream(int64_t index);
        int64_t             streamAmount() const;
        void                setStreams(StreamVector stream_list);

        void                stampPacket(Packet& packet);

        virtual std::string toString() const override final;

        //**** refactoring


        Preset              preset() const;                 //TODO убрать 24.01
        bool                presetIs(Preset value) const;   //TODO убрать 24.01

    private:

        /* ? */
        enum InterruptedProcess {
            None,
            Opening,
            Closing,
            Reading,
            Writing,
        };

        class Interrupter {

        public:

            Interrupter(InterruptedProcess process)
                : interrupted_process(process) {
            }

            InterruptedProcess  interrupted_process;
            Chronometer         chronometer;

        };

    protected:

        virtual Code        createContext() = 0;
        virtual Code        openContext()   = 0;
        virtual Code        closeContext()  = 0;

        void                setFormatContext(SharedAVFormatContext format_context);

    private:

        void                setOpened(bool opened);
        void                setInteruptCallback(InterruptedProcess process);
        void                resetInteruptCallback();
        static int          interrupt_callback(void* opaque);

    private:

        SharedAVFormatContext   _format_context;
        const std::string       _media_resource_locator;
        const Preset            _preset;
        bool                    _opened;
        StreamVector            _streams;
        Interrupter             _current_interrupter;

    };

    enum class ReadWriteMode {
        Instant,
        Interleaved,
    };

} // namespace fpp
