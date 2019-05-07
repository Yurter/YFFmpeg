#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}

#include <string>
#include <thread>

class YAbstractMedia
{

public:

    YAbstractMedia(const std::string &mrl);                             // mrl - media resource locator.
    virtual ~YAbstractMedia();

    virtual bool        open() = 0;                                     // Функция открывает медиа-ресурс.
    virtual bool        close() = 0;									// Функция закрывает медиа-ресурс.
    virtual bool        isActive() const;								// Функция возвращает true, если запись/чтение не закончены, иначе - false.

    std::string         mediaResourceLocator() const;                   // Функция возвращает mrl.
    AVFormatContext*    mediaFormatContext() const;                     // Функция возвращает медиа-контекст.
    uint64_t            width() const;									// Функция возвращает ширину видео-кадрa.
    uint64_t            height() const;									// Функция возвращает высоту видео-кадрa.
    AVRational          aspectRatio() const;                            //
    uint64_t            frameRate() const;								// Функция возвращает частоту кадров видео-потока.
    uint64_t            duration() const;								// Функция возвращает длительность медиа-файла в секундах.
    std::string         format() const;									// Функция возвращает формат медиа-файла.
    std::string         videoCodecName() const;							// Функция возвращает название видео-кодека.
    std::string         audioCodecName() const;							// Функция возвращает название аудио-кодека.
    bool                videoStreamAvailable() const;					// Функция возвращает true, если у медиа-файла имеется видео-поток, иначе - false.
    bool                audioStreamAvailable() const;					// Функция возвращает true, если у медиа-файла имеется аудио-поток, иначе - false.

    void                setReopeingAfterFailure(bool reopening_after_failure);
//    void                setReopeningTimeout(uint64_t reopening_timeout);
    void                setWidth(uint64_t width);
    void                setHeight(uint64_t height);
    void                setAspectRatio(AVRational aspect_ratio);
    void                setFrameRate(uint64_t frame_rate);
    void                setFormat(std::string format);
    void                setVideoCodecName(std::string video_codec_name);
    void                setAudioCodecName(std::string audio_codec_name);

    // Варианты для быстрой предустновки медиа-ресурсов.
    enum YMediaPreset {
        Auto,
        /* Input */
        Silence,
        /* Output */
        YouTube,
        Timelapse
    };

protected:

    void                getInfo();
    std::string         guessFormatName();

protected:

	// General parameters
    std::thread         _thread;
	std::string			_media_resource_locator;
	bool				_is_active;
	bool				_is_opened;
    bool                _reopening_after_failure;
//    uint64_t            _reopening_timeout;

	// FFmpeg 
	AVFormatContext*	_media_format_context;
	AVFormatContext*	_lavfi_video_format_context;
	AVFormatContext*	_lavfi_audio_format_context;

	// Media parameters
	uint64_t			_width;
	uint64_t			_height;
    AVRational          _aspect_ratio;
	std::string			_format;
	uint64_t			_duration;
    uint64_t    		_frame_rate;
    AVCodecID           _video_codec_id;
    AVCodecID           _audio_codec_id;
	std::string			_video_codec_name;
	std::string			_audio_codec_name;
	bool				_video_available;
	bool				_audio_available;

};
