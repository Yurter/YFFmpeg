#pragma once
#include "DataProcessor.hpp"
#include "../control/VideoStream.hpp"
#include "../control/AudioStream.hpp"

namespace fpp {

    class Context : public DataProcessor<Packet, Packet> {

    public:

        Context(const std::string& mrl, MediaPreset preset = MediaPreset::Auto); ///< mrl - media resource locator.
        Context(const Context& other)  = delete;
        Context(const Context&& other) = delete;
        virtual ~Context() override;

        virtual Code        open() = 0;                                 ///< Функция открывает медиа-контекст.
        virtual Code        close();                                    ///< Функция закрывает медиа-контекст.
        bool                opened() const;                             ///< Функция возвращает true, если контекст находится в открытом состоянии.
        bool                closed() const;                             ///< Функция возвращает true, если контекст находится в закрытом состоянии.

        void                setOpened(bool opened);                     ///< ...

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

        StreamVector        streams();                                  ///< Функция возвращает все потоки текущего контекста.
        StreamVector        streams(MediaType media_type);              ///< Функция потоки заданного медиа-типа.

    protected:

        virtual Code        createContext() = 0;
        virtual Code        openContext()   = 0;

        Code                parseFormatContext();

    private:

        void                setUid(int64_t uid);                        ///< Функция установки uid контекста, не допускает повторного вызовова.

    protected:

        // General
        int64_t             _uid;
        std::string			_media_resource_locator;
        bool				_opened;
        StreamVector        _streams;
        Thread              _io_thread;
        bool                _reopening_after_failure;
        int64_t             _reopening_timeout;
        int64_t             _artificial_delay;
        MediaPreset         _preset;

        // FFmpeg
        AVFormatContext*	_format_context;

    };

    using ContextList = std::list<Context*>;

} // namespace fpp
