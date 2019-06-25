#pragma once

#include "YDataProcessor.h"
#include "../control/YVideoParameters.h"
#include "../control/YAudioParameters.h"
#include "../control/YVideoStream.h"
#include "../control/YAudioStream.h"

class YAbstractMedia : public YDataProcessor<YPacket, YPacket>
{

public:

    YAbstractMedia(const std::string& mrl);                             // mrl - media resource locator.
    virtual ~YAbstractMedia();

    virtual YCode       open() = 0;                                     // Функция открывает медиа-ресурс.
    virtual YCode       close();                                        // Функция закрывает медиа-ресурс.
    virtual bool        opened() const final;                           //
    virtual bool        closed() const final;                           //

    YCode               createDummyStream(YMediaType type, YParameters parametres);           //
    YCode               createStream(AVCodecContext* codec_context);    //TODO

    void                setUid(int64_t uid);                            //
    void                setReopeingAfterFailure(bool reopening);        //
    void                setReopeningTimeout(uint64_t timeout);          //

    int64_t             uid() const;                                    //
    std::string         mediaResourceLocator() const;                   // Функция возвращает mrl.
    AVFormatContext*    mediaFormatContext() const;                     // Функция возвращает медиа-контекст.
    int64_t             duration() const;								// Функция возвращает длительность медиа-файла в секундах.
    YStream*            stream(int64_t index);                          //

protected:

    YCode               createContext();
    virtual YCode       openContext() = 0;
    YCode               parseFormatContext();
    std::string         guessFormatShortName();

public:

    // Media
//    YVideoParameters    video_parameters;
//    YAudioParameters    audio_parameters;

protected:

    // General
    int64_t             _uid;
	std::string			_media_resource_locator;
    bool				_opened;

    bool                _reopening_after_failure;
    int64_t             _reopening_timeout;

    int64_t             _artificial_delay;

    YThread             _io_thread;

    std::vector<YStream>_streams;

    // FFmpeg
    AVFormatContext*	_media_format_context;

};
