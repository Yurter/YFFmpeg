#pragma once

#include "ffmpeg.h"

#include <string>
#include <thread>
#include <mutex>

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

class YAbstractMedia
{

public:

    YAbstractMedia(const std::string &mrl);                             // mrl - media resource locator.
    virtual ~YAbstractMedia();

    virtual bool        open() = 0;                                     // Функция открывает медиа-ресурс.
    virtual bool        close();                                        // Функция закрывает медиа-ресурс.
    virtual bool        active() const final;                           // Функция возвращает true, если запись/чтение не закончены, иначе - false.
    virtual bool        opened() const final;                           //

    std::string         mediaResourceLocator() const;                   // Функция возвращает mrl.
    AVFormatContext*    mediaFormatContext() const;                     // Функция возвращает медиа-контекст.
    AVInputFormat*      inputFormat() const;
    AVOutputFormat*     outputFormat() const;
    int64_t             width() const;									// Функция возвращает ширину видео-кадрa.
    int64_t             height() const;									// Функция возвращает высоту видео-кадрa.
    AVRational          aspectRatio() const;                            //
    int64_t             sampleRate() const;                             //
    AVSampleFormat      sampleFormat() const;                           //
    int64_t             audioBitrate() const;                           //
    uint64_t            audioChanelsLayout() const;                     //
    int64_t             audioChanels() const;                           //
    uint64_t            frameRate() const;								// Функция возвращает частоту кадров видео-потока.
    int64_t             duration() const;								// Функция возвращает длительность медиа-файла в секундах.
    int64_t             videoDuration() const;                          // Функция возвращает длительность видео-потока.
    int64_t             audioDuration() const;							// Функция возвращает длительность аудио-потока.
    std::string         videoCodecName() const;							// Функция возвращает название видео-кодека.
    std::string         audioCodecName() const;							// Функция возвращает название аудио-кодека.
    AVCodecID           videoCodecId() const;							// Функция возвращает название видео-кодека.
    AVCodecID           audioCodecId() const;							// Функция возвращает название аудио-кодека.
    bool                videoAvailable() const;                         // Функция возвращает true, если у медиа-файла имеется видео-поток, иначе - false.
    bool                audioAvailable() const;                         // Функция возвращает true, если у медиа-файла имеется аудио-поток, иначе - false.
    int64_t             videoStreamIndex() const;                       // Функция возращает индекс видео-потока, -1 если отсутствует.
    int64_t             audioStreamIndex() const;                       // Функция возращает индекс аудио-потока, -1 если отсутствует.

    void                setReopeingAfterFailure(bool reopening_after_failure);
    void                setReopeningTimeout(uint64_t reopening_timeout);
    void                setWidth(int64_t width);
    void                setHeight(int64_t height);
    void                setDuration(int64_t duration);
    void                setVideoDuration(int64_t video_duration);
    void                setAudioDuration(int64_t audio_duration);
    void                setAspectRatio(AVRational aspect_ratio);
    void                setFrameRate(uint64_t frame_rate);
    void                setFrameRate(AVRational frame_rate);
    void                setVideoCodecName(std::string video_codec_name);
    void                setAudioCodecName(std::string audio_codec_name);
    void                setVideoCodecId(AVCodecID video_codec_id);
    void                setAudioCodecId(AVCodecID audio_codec_id);
    void                setSampleRate(int64_t sample_rate);
    void                setSampleFormat(AVSampleFormat sample_format);
    void                setVideoBitrate(int64_t video_bitrate);
    void                setAudioBitrate(int64_t audio_bitrate);
    void                setAudioChanelsLayout(uint64_t chanels_layout);
    void                setAudioChanels(int64_t chanels);

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

    virtual bool        run() = 0;

    void                parseFormatContext();
    std::string         guessFormatShortName();

protected:

	// General parameters
    std::thread         _thread;
	std::string			_media_resource_locator;
    bool				_is_opened;
	bool				_is_active;
    bool                _reopening_after_failure;
    uint64_t            _reopening_timeout;
    bool                _close_after_failure;
    uint64_t            _close_timeout;

	// FFmpeg 
    AVFormatContext*	_media_format_context;
    AVInputFormat*      _input_format;
    AVOutputFormat*     _output_format;

	// Media parameters
    /* General */
    std::string			_format_short_name;
    bool				_video_available;
    bool				_audio_available;
    int64_t             _video_stream_index;
    int64_t             _audio_stream_index;
    int64_t             _duration;
    /* Video */
    AVCodecID           _video_codec_id;
    std::string			_video_codec_name;
    int64_t             _width;
    int64_t             _height;
    AVRational          _aspect_ratio;
    uint64_t    		_frame_rate;
    int64_t             _video_bitrate;
    int64_t             _video_duration;
    /* Audio */
    AVCodecID           _audio_codec_id;
    std::string			_audio_codec_name;
    int64_t             _sample_rate;
    AVSampleFormat      _sample_format;
    int64_t             _audio_bitrate;
    int64_t             _audio_duration;
    uint64_t            _chanels_layout;
    int64_t             _chanels;


};
