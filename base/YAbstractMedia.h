#pragma once

#include "ffmpeg.h"
#include "YVideoParameters.h"
#include "YAudioParameters.h"
#include "YPacket.h"

#include <string>
#include <thread>
#include <mutex>
#include <queue>

class YAbstractMedia
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
    AVStream*           stream(int64_t index);                          //
    int64_t             duration() const;								// Функция возвращает длительность медиа-файла в секундах.

protected:

    virtual void        run() = 0;
    void                stopThread();

    void                queuePacket(YPacket packet);
    bool                getPacket(YPacket &packet);

    void                parseFormatContext();
    std::string         guessFormatShortName();

public:

    // Media parameters
    YVideoParameters    video_parameters;
    YAudioParameters    audio_parameters;

protected:

	// General parameters
    std::thread         _thread;
	std::string			_media_resource_locator;
    bool				_opened;
    volatile bool       _running;
    bool                _reopening_after_failure;
    int64_t             _reopening_timeout;
    bool                _close_after_failure;
    int64_t             _close_timeout;
    int64_t             _artificial_delay;

    std::queue<YPacket>     _packet_queue;
    std::mutex              _packet_queue_mutex;
    uint64_t                _packet_queue_capacity;

	// FFmpeg 
    AVFormatContext*	_media_format_context;

};
