#pragma once
#include "core/utils.hpp"
#include "core/time/Chronometer.hpp"

namespace fpp {

    enum class ReadWriteMode {
        Instant,
        Interleaved,
    };

    class FormatContext : public Object {

    public:

        FormatContext(const std::string mrl, IOPreset preset = IOPreset::Auto); ///< mrl - media resource locator.
        FormatContext(const FormatContext& other)  = delete;
        virtual ~FormatContext() override;

        IOPreset            preset() const;
        bool                presetIs(IOPreset value) const;

        Code                open();
        Code                close();
        bool                opened() const;
        bool                closed() const;

        std::string         mediaResourceLocator()  const;  ///< Функция возвращает mrl.
        AVFormatContext*    mediaFormatContext()    const;  ///< Функция возвращает ffmpeg медиа-контекст.

        AVFormatContext**   mediaFormatContext2(); // TODO 14.01

        AVStream*           stream(int64_t index);          ///< Функция возвращает указатель на поток с заданным индексом; nullptr, если невалидный индекс.
        int64_t             numberStream()  const;          ///< Функция возвращает количество потоков в текущем котексте.
        AVInputFormat*      inputFormat()   const;          ///< Функция ...
        AVOutputFormat*     outputFormat()  const;          ///< Функция ...

    private:

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

        Code                setMediaFormatContext(AVFormatContext* value);

    private:

        void                setOpened(bool opened);
        void                setInteruptCallback(InterruptedProcess process);
        void                resetInteruptCallback();
        static int          interrupt_callback(void* opaque);

    private:

        AVFormatContext*	_format_context;
        const std::string   _media_resource_locator;
        const IOPreset      _preset;
        bool                _opened;
        int64_t             _artificial_delay;
        Interrupter         _current_interrupter;

    };

} // namespace fpp
