#pragma once

#include "ffmpeg.h"
#include "YVideoParameters.h"
#include "YAudioParameters.h"
#include "YAsyncQueue.h"
#include "YPacket.h"
#include "YThread.h"
#include "YStream.h"

#include <string>
#include <thread>
#include <mutex>
#include <queue>

class YAbstractMedia : public YThread
{

public:

    YAbstractMedia(const std::string &mrl);                             // mrl - media resource locator.
    virtual ~YAbstractMedia();

    virtual bool        open() = 0;                                     // Функция открывает медиа-ресурс.
    virtual bool        close();                                        // Функция закрывает медиа-ресурс.
    virtual bool        opened() const final;                           //
    virtual bool        closed() const final;                           //

    void                setReopeingAfterFailure(bool reopening);        //
    void                setReopeningTimeout(uint64_t timeout);          //

    std::string         mediaResourceLocator() const;                   // Функция возвращает mrl.
    AVFormatContext*    mediaFormatContext() const;                     // Функция возвращает медиа-контекст.
    YStream             stream(uint64_t index);                         //
    std::vector<YStream>* streams();                         //
    int64_t             duration() const;								// Функция возвращает длительность медиа-файла в секундах.

    void                push(YPacket packet);
    bool                pop(YPacket &packet);

protected:

    void                parseFormatContext();
    std::string         guessFormatShortName();

public:

    // Media
    YVideoParameters    video_parameters;
    YAudioParameters    audio_parameters;

    std::vector<YStream>    _streams;

protected:

    // General
	std::string			_media_resource_locator;
    bool				_opened;
    bool                _reopening_after_failure;
    int64_t             _reopening_timeout;
    bool                _close_after_failure;
    int64_t             _close_timeout;
    int64_t             _artificial_delay;

    YAsyncQueue<YPacket>    _packet_queue;

	// FFmpeg 
    AVFormatContext*	_media_format_context;

};
