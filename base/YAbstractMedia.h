#pragma once

#include "YDataProcessor.h"
#include "../control/YVideoStream.h"
#include "../control/YAudioStream.h"

typedef std::vector<YStream*> Streams;

class YAbstractMedia : public YDataProcessor<YPacket, YPacket>
{

public:

    YAbstractMedia(const YAbstractMedia& other) = delete;
    YAbstractMedia(const YAbstractMedia&& other) = delete;
    YAbstractMedia(const std::string& mrl);                             // mrl - media resource locator.
    virtual ~YAbstractMedia();

    virtual YCode       open() = 0;                                     // Функция открывает медиа-ресурс.
    virtual YCode       close();                                        // Функция закрывает медиа-ресурс.
    virtual bool        opened() const final;                           //
    virtual bool        closed() const final;                           //

    YCode               createStream(YStream* new_stream);               //
    YStream*            bestStream(YMediaType type);                    //

    void                setUid(int64_t uid);                            //
    void                setReopeingAfterFailure(bool reopening);        //
    void                setReopeningTimeout(uint64_t timeout);          //

    int64_t             uid() const;                                    //
    std::string         mediaResourceLocator() const;                   // Функция возвращает mrl.
    AVFormatContext*    mediaFormatContext() const;                     // Функция возвращает медиа-контекст.
    int64_t             duration() const;								// Функция возвращает длительность медиа-файла в секундах.
    YStream*            stream(int64_t index);                          //
    int64_t             numberStream() const;                           //

protected:

    YCode               createContext();
    YCode               attachStreams();
    virtual YCode       openContext() = 0;
    YCode               parseFormatContext();
    std::string         guessFormatShortName();

protected:

    // General
    int64_t             _uid;
	std::string			_media_resource_locator;
    bool				_opened;

    Streams             _streams;
    YThread             _io_thread;

    bool                _reopening_after_failure;
    int64_t             _reopening_timeout;

    int64_t             _artificial_delay;

    // FFmpeg
    AVFormatContext*	_media_format_context;

};

typedef std::pair<YAbstractMedia*,int64_t> stream_context;
