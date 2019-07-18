#pragma once

#include "YDataProcessor.hpp"
#include "../control/YVideoStream.hpp"
#include "../control/YAudioStream.hpp"



class YContext : public YDataProcessor<YPacket, YPacket>
{

public:

    YContext(const std::string& mrl, YMediaPreset preset = YMediaPreset::Auto); ///< mrl - media resource locator.
    YContext(const YContext& other) = delete;
    YContext(const YContext&& other) = delete;
    virtual ~YContext() override;

    virtual YCode       open() = 0;                                 ///< Функция открывает медиа-контекст.
    virtual YCode       close();                                    ///< Функция закрывает медиа-контекст.
    virtual bool        opened() const final;                       ///< Функция возвращает true, если контекст находится в открытом состоянии.
    virtual bool        closed() const final;                       ///< Функция возвращает true, если контекст находится в закрытом состоянии.

    YCode               createStream(YStream* new_stream);          ///< Функция создает поток к текущем контексте.
    YCode               createStream(YParameters* param);           ///< Функция создает поток к текущем контексте.
//    YStream*            bestStream(YMediaType type);                ///< Функция возвращает указатель на поток заданного типа с наилучшими параметрами; nullptr, если потока заданного типа нет.
    YVideoStream*       bestVideoStream();
    YAudioStream*       bestAudioStream();
    void                reopenAfterFailure(int64_t timeout);        ///< Функция позволяет автоматически переоткрывать контекст в случаее его закрытия по заданному таймауту в секундах.

    int64_t             uid() const;                                ///< Функция возвращает uid.
    std::string         mediaResourceLocator() const;               ///< Функция возвращает mrl.
    AVFormatContext*    mediaFormatContext() const;                 ///< Функция возвращает ffmpeg медиа-контекст.
    int64_t             duration() const;							///< Функция возвращает длительность медиа-файла в секундах.
    YStream*            stream(int64_t index);                      ///< Функция возвращает указатель на поток с заданным индексом; nullptr, если невалидный индекс.
    int64_t             numberStream() const;                       ///< Функция возвращает количество потоков в текущем котексте.

    AVInputFormat*      inputFormat()   const;
    AVOutputFormat*     outputFormat()  const;

    bool                supportsVideo();
    bool                supportsAudio();

    StreamVector        streams(YMediaType media_type);

protected:

    virtual YCode       createContext() = 0;
    virtual YCode       openContext()   = 0;

    YCode               parseFormatContext();

private:

    void                setUid(int64_t uid);                        ///< Функция установки uid контекста, не допускает повторного вызовова.
//    YStream*            bestVideoStream()

protected:

    // General
    int64_t             _uid;
	std::string			_media_resource_locator;
    bool				_opened;
    StreamVector        _streams;
    YThread             _io_thread;
    bool                _reopening_after_failure;
    int64_t             _reopening_timeout;
    int64_t             _artificial_delay;
    YMediaPreset        _preset;

    // FFmpeg
    AVFormatContext*	_format_context;

};

typedef std::pair<YContext*,int64_t> stream_context;
