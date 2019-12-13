#pragma once
#include "stream/VideoStream.hpp"
#include "stream/AudioStream.hpp"
#include "core/utils.hpp"
#include "core/Chronometer.hpp"

namespace fpp {

    class FormatContext : public Object {

    public:

        FormatContext(const std::string mrl, /*ProcessorPointer* stream_context,*/ IOType preset = IOType::Auto); ///< mrl - media resource locator.
        FormatContext(const FormatContext& other)  = delete;
        FormatContext(const FormatContext&& other) = delete;
        virtual ~FormatContext() override;

        IOType              preset() const;
        bool                preset(IOType value) const;

        Code                open();
        Code                close();

        bool                opened() const;
        bool                closed() const;

        Code                createStream(Stream* new_stream);           ///< Функция создает поток к текущем контексте.
        Code                createStream(Parameters* param);            ///< Функция создает поток к текущем контексте.
        Stream*             bestStream(MediaType type);                 ///< Функция возвращает указатель на поток заданного типа с наилучшими параметрами; nullptr, если потока заданного типа нет.
        void                reopenAfterFailure(int64_t timeout);        ///< Функция позволяет автоматически переоткрывать контекст в случаее его закрытия по заданному таймауту в секундах.

        int64_t             uid() const;                                ///< Функция возвращает uid.
        std::string         mediaResourceLocator() const;               ///< Функция возвращает mrl.
        AVFormatContext*    mediaFormatContext() const;                 ///< Функция возвращает ffmpeg медиа-контекст.
        int64_t             duration() const;							///< Функция возвращает длительность медиа-файла в секундах.
        Stream*             stream(int64_t index);                      ///< Функция возвращает указатель на поток с заданным индексом; nullptr, если невалидный индекс.
        int64_t             numberStream() const;                       ///< Функция возвращает количество потоков в текущем котексте.

        AVInputFormat*      inputFormat()   const;                      ///< Функция ...
        AVOutputFormat*     outputFormat()  const;                      ///< Функция ...

        bool                supportsVideo();                            ///< Функция ...
        bool                supportsAudio();                            ///< Функция ...

        StreamVector        streams() const;                                  ///< Функция возвращает все потоки текущего контекста.
        StreamVector        streams(MediaType media_type);              ///< Функция потоки заданного медиа-типа.

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

        Code                parseFormatContext();

    private:

        void                setOpened(bool opened);
        void                setUid(int64_t uid);                        ///< Функция установки uid контекста, не допускает повторного вызовова.

        void                setInteruptCallback(InterruptedProcess process);
        void                resetInteruptCallback();
        static int          interrupt_callback(void* opaque);

    protected:

        int64_t             _uid;
        std::string			_media_resource_locator;
        std::atomic_bool	_opened;
        StreamVector        _streams;
//        ProcessorPointer*   _stream_context;
        bool                _reopening_after_failure;
        int64_t             _reopening_timeout;
        int64_t             _artificial_delay;
        IOType              _preset;
        Interrupter         _current_interrupter;

        // FFmpeg
        AVFormatContext*	_format_context;

    };

//    using FormatContextList = std::list<FormatContext*>;

} // namespace fpp
