#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/mathematics.h>
#include <libavutil/channel_layout.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/avfilter.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

/* ? */
//enum Yode {hwaccel
//    //
//};

/* Варианты для быстрой преднастройки */
/* параметров медиа-контекстов        */
enum YMediaPreset {
    Auto,
    /* Input */
    Virtual,
    Memory,
    /* Output */
    YouTube,
    Timelapse
};

/* Коды результата выполнения некоторых функций */
enum YCode {
    OK,
    ERR,
    AGAIN,
    NOT_INITED,
    END_OF_FILE,
    INVALID_INPUT,
    INVALID_CALL_ORDER,
    FFMPEG_ERROR,
    EXCEPTION
    //TODO код возрата без ошибки, что-то типа варнинга
};

/* Опции для YFFmpeg */
enum YOption {
    COPY_VIDEO      = 0x0001,
    COPY_AUDIO      = 0x0002,
    VIDEO_REQUIRED  = 0x0004,
    AUDIO_REQUIRED  = 0x0008,
};

/* Медиа тип потока/пакета/фрейма */
enum YMediaType {
    MEDIA_TYPE_UNKNOWN  = 0x0001,
    MEDIA_TYPE_VIDEO    = 0x0002,
    MEDIA_TYPE_AUDIO    = 0x0004,
};

/* Значения по умолчанию для видео- и аудиопараметров */ //TODO по умолчанию невалидные, добавить по умолчанию валидные
#define INVALID_INT             -1
#define DEFAULT_INT             0
//#define DEFAULT_UINT            0 //TODO ?
#define DEFAULT_FLOAT           0.f
#define DEFAULT_STRING          "none"
#define DEFAULT_RATIONAL        { 0, 1 }
#define DEFAULT_CODEC_ID        AV_CODEC_ID_NONE
#define DEFAULT_PIXEL_FORMAT    AV_PIX_FMT_NONE
#define DEFAULT_SAMPLE_FORMAT   AV_SAMPLE_FMT_NONE
#define DEFAULT_CHANEL_LAYOUT   0

#define DEFAULT_TIME_BASE       { 1, 1000 }

#define invalid_int(x)          ((x) == INVALID_INT)
#define invalid_rational(x)     ((x.den) == 0)
#define not_inited_int(x)       ((x) == DEFAULT_INT)
#define not_inited_ptr(x)       ((x) == nullptr)
#define not_inited_float(x)     ((x) == DEFAULT_FLOAT)
#define not_inited_string(x)    ((x) == DEFAULT_STRING)
#define not_inited_q(x)         (av_cmp_q(x, DEFAULT_RATIONAL) == 0)
#define not_inited_codec_id(x)  ((x) == DEFAULT_CODEC_ID)
#define not_inited_pix_fmt(x)   ((x) == DEFAULT_PIXEL_FORMAT)
#define not_inited_smp_fmt(x)   ((x) == DEFAULT_SAMPLE_FORMAT)
#define not_inited_ch_layout(x) ((x) == DEFAULT_CHANEL_LAYOUT)

#define inited_int(x)           ((x) != DEFAULT_INT)
#define inited_ptr(x)           ((x) != nullptr)
#define inited_codec_id(x)      ((x) != DEFAULT_CODEC_ID)

/* Категории сообщений, которые выводятся в консоль.
 * Каждый последующий уровень включает в себя предыдущий */
enum YLogLevel {
    /* Сообщения не выводится */
    Quiet,
    /* Сообщения об ошибках */
    Error,
    /* Сообщения о некорректно установленных параметрах,
     * которые могут привести к проблемам */
    Warning,
    /* Стандартная информация */
    Info,
    /* Сообщения, используемые при отладке кода */
    Debug
};

/* ? */
#define guaranteed_push(proc,data) while (!proc->push(data)) { utils::sleep_for(SHORT_DELAY_MS); } SEMICOLON_REQUIREMENT
#define guaranteed_pop(proc,data)  while (!proc->pop(data))  { utils::sleep_for(SHORT_DELAY_MS); } SEMICOLON_REQUIREMENT

/* Функция возвращает id потока, в котором вызвана */
#define current_thread_id() std::this_thread::get_id()

/* ? */
#define try_to(x) { auto ret = x;\
                    if (utils::exit_code(ret)) {\
                        log_error("Function " << (#x)\
                        << " failed with code: " << ret\
                        << " - " << utils::code_to_string(ret));\
                        return ret;\
                    }\
                  } SEMICOLON_REQUIREMENT

/* ? */
//TODO
// Возрат кода и отправка сообщения в лог, сответствующий коду

/* ? */
#define return_if(cond,ret_value) { if (cond) { return ret_value; } } SEMICOLON_REQUIREMENT

/* ? */
#define return_if_not(cond,ret_value) { if (!(cond)) { return ret_value; } } SEMICOLON_REQUIREMENT

/* ? */
#define trow_if(cond,error_message) //TODO

/* ? */
#define if_not(x) if(!(x))

/* Значения для задержек в коде в мс */
#define SHORT_DELAY_MS          10
#define MEDIUM_DELAY_MS         100
#define LONG_DELAY_MS           1000

/* ? */
//#define TAB                     "\t"
#define TAB                     "  "

/* ? */
#define SEMICOLON_REQUIREMENT   void(0)

/* ? */
#define EMPTY_CONSTRUCTOR
