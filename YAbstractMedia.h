#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}

#include <string>

class YAbstractMedia
{
public:

    YAbstractMedia(const std::string &mrl);                    // mrl - media resource locator.
    virtual ~YAbstractMedia();

    virtual bool open() = 0;                                    // Функция открывает медиа-ресурс.
    virtual bool close() = 0;									// Функция закрывает медиа-ресурс.
    virtual bool isActive() const;								// Функция возвращает true, если запись/чтение не закончены, иначе - false.

    std::string         mediaResourceLocator() const;           // Функция возвращает mrl.
    AVFormatContext*    mediaFormatContext() const;             //
	uint64_t	width() const;									// Функция возвращает ширину видео-кадрa.
	uint64_t	height() const;									// Функция возвращает высоту видео-кадрa.
	float		frameRate() const;								// Функция возвращает частоту кадров видео-потока.
	uint64_t	duration() const;								// Функция возвращает длительность медиа-файла в секундах.
	std::string format() const;									// Функция возвращает формат медиа-файла.
	std::string videoCodec() const;								// Функция возвращает название видео-кодека.
	std::string audioCodec() const;								// Функция возвращает название аудио-кодека.
	bool		videoStreamAvailable() const;					// Функция возвращает true, если у медиа-файла имеется видео-поток, иначе - false.
	bool		audioStreamAvailable() const;					// Функция возвращает true, если у медиа-файла имеется аудио-поток, иначе - false.

	void		setWidth(uint64_t width);
	void		setHeight(uint64_t height);
	void		setFrameRate(float frame_rate);
    void        setFormat(std::string format);
    void        setVideoCodecName(std::string video_codec_name);
    void        setAudioCodecName(std::string audio_codec_name);

    //
    enum YMediaPreset {
        Auto,
        YouTube,
        Timelapse
    };

protected:

    void            getInfo();
    std::string     guessFormatName();

protected:

	// General parameters
	std::string			_media_resource_locator;
	bool				_is_active;
	bool				_is_opened;

	// FFmpeg 
	AVFormatContext*	_media_format_context;
	AVFormatContext*	_lavfi_video_format_context;
	AVFormatContext*	_lavfi_audio_format_context;

	// Media parameters
	uint64_t			_width;
	uint64_t			_height;
	std::string			_format;
	uint64_t			_duration;
	float				_frame_rate;
    AVCodecID           _video_codec_id;
    AVCodecID           _audio_codec_id;
	std::string			_video_codec_name;
	std::string			_audio_codec_name;
	bool				_video_available;
	bool				_audio_available;

};
