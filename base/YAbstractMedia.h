#pragma once

#include "ffmpeg.h"
#include "YDataProcessor.h"
#include "YVideoParameters.h"
#include "YAudioParameters.h"
#include "YPacket.h"
#include "YStream.h"
#include <string>

class YAbstractMedia : public YDataProcessor<YPacket, YPacket>
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
    int64_t             duration() const;								// Функция возвращает длительность медиа-файла в секундах.
    YStream*            stream(uint64_t index);                         //

protected:

    void                parseFormatContext();
    std::string         guessFormatShortName();

public:

    // Media
    YVideoParameters    video_parameters;
    YAudioParameters    audio_parameters;

protected:

    // General
	std::string			_media_resource_locator;
    bool				_opened;

    bool                _reopening_after_failure;
    int64_t             _reopening_timeout;

    bool                _close_after_failure; //TODO нужно?
    int64_t             _close_timeout;

    int64_t             _artificial_delay;

    YThread             _io_thread;

    std::vector<YStream>    _streams;

    // FFmpeg
    AVFormatContext*	_media_format_context;

};
