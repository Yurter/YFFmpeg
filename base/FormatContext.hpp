#pragma once
#include <core/Object.hpp>
#include <core/time/Chronometer.hpp>
#include <stream.hpp>
#include <memory>

namespace ffmpeg {
    struct AVFormatContext;
    struct AVStream;
    struct AVInputFormat;
    struct AVOutputFormat;
} // namespace ffmpeg

namespace fpp {

    enum class ReadWriteMode {
        Instant,
        Interleaved,
    };

    /* Варианты для быстрой преднастройки */
    /* параметров медиа-контекстов        */
    enum Preset {
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

        FormatContext(const std::string& mrl, Preset preset); ///< mrl - media resource locator.
        FormatContext(const FormatContext& other) = delete;
        virtual ~FormatContext() override;

        Preset              preset() const;
        bool                presetIs(Preset value) const;

        Code                open();
        Code                close();
        bool                opened() const;
        bool                closed() const;

        virtual std::string toString() const override final;

        std::string         mediaResourceLocator()  const;  ///< Функция возвращает mrl.
        SharedAVFormatContext   formatContext()    const;  ///< Функция возвращает ffmpeg медиа-контекст.

        void                setStreams(StreamVector stream_list);
        StreamVector        streams();
        const StreamVector  streams() const;
        const ffmpeg::AVStream* stream(int64_t index);          ///< Функция возвращает указатель на поток с заданным индексом; nullptr, если невалидный индекс. //TODO переделать AVStream на Stream 20.01
        int64_t             numberStream()  const;          ///< Функция возвращает количество потоков в текущем котексте.
        ffmpeg::AVInputFormat*      inputFormat()   const;          ///< Функция ...
        ffmpeg::AVOutputFormat*     outputFormat()  const;          ///< Функция ...

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

            Interrupter(InterruptedProcess process, FormatContext* caller) :
                interrupted_process(process)
              , caller_object(caller) {}

            InterruptedProcess  interrupted_process;
            FormatContext*      caller_object;
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
        int64_t                 _artificial_delay;
        Interrupter             _current_interrupter;

    };

} // namespace fpp
