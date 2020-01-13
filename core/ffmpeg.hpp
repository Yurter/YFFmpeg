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

namespace fpp {

    /* Варианты для быстрой преднастройки */
    /* параметров медиа-контекстов        */
    enum IOType {
        Auto,
        Raw,
        /* Input */
        Virtual,
        /* Output */
        Event, //TODO костыльный тип ради создания потоков у синков, убрать (нет, необходимо жестко задавать видео- и аудиопотоки)
        YouTube,
        Timelapse,
        OpenCV,
    };

    /* Коды результата выполнения некоторых функций */
    enum /*class*/ [[nodiscard]] Code {
        OK,
        ERR,
        EXIT,
        AGAIN,
        EXCEPTION,
        NOT_INITED,
        END_OF_FILE,
        FFMPEG_ERROR,
        INVALID_INPUT,
        NOT_IMPLEMENTED,
        INVALID_CALL_ORDER,
    };

    /* Опции для Pipeline */
    enum Option {
        COPY_VIDEO      = 1 << 0x0001,
        COPY_AUDIO      = 1 << 0x0002,
        VIDEO_REQUIRED  = 1 << 0x0003,
        AUDIO_REQUIRED  = 1 << 0x0004,
    };

    /* ? */
    enum class CodecType { //TODO конфликт имён
        Unknown,
        Decoder,
        Encoder,
    };

    enum class ProcessorType {
        Unknown,
        Process,
        Output,
        Input,
    };

    /* Медиа тип потока/пакета/фрейма */
    enum MediaType {
        MEDIA_TYPE_UNKNOWN  = 0x0001,
        MEDIA_TYPE_VIDEO    = 0x0002,
        MEDIA_TYPE_AUDIO    = 0x0004,
        MEDIA_TYPE_EOF      = 0x0008,
    };

    /* Категории сообщений, которые выводятся в консоль.
     * Каждый последующий уровень включает в себя предыдущий */
    enum LogLevel {
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
        Debug,
        /* ? */
        Verbose, //TODO вынести часть лога в этот раздел из дебага
        /* Чрезвычайно подробный лог, полезный при разработке fpp */
        Trace,
    };

    /* ? */
    enum InterruptedProcess {
        None,
        Opening,
        Closing,
        Reading,
        Writing,
    };

} // namespace fpp

#define UNUSED(x) (void)(x)

/* Значения по умолчанию для видео- и аудиопараметров */ //TODO по умолчанию невалидные, добавить по умолчанию валидные
#define INVALID_INT             -1
#define DEFAULT_INT             0
#define DEFAULT_FLOAT           0.f
#define DEFAULT_STRING          "none"
#define DEFAULT_RATIONAL        AVRational { 0, 1 }
#define DEFAULT_CODEC_ID        AV_CODEC_ID_NONE
#define DEFAULT_PIXEL_FORMAT    AV_PIX_FMT_NONE
#define DEFAULT_SAMPLE_FORMAT   AV_SAMPLE_FMT_NONE
#define DEFAULT_CHANEL_LAYOUT   0
#define DEFAULT_TIME_BASE       AVRational { 1, 1000 }
#define DEFAULT_SAMPLE_RATE     44'100

#define FROM_START              0
#define TO_END                  LONG_MAX

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

/* Функция возвращает id потока, в котором вызвана */
#define current_thread_id() std::this_thread::get_id()

/* ? */
//#define try_to(x)   do { auto ret = x;\
//                        if (utils::exit_code(ret)) {\
//                            log_error("Function " << (#x)\
//                            << " failed with code: " << ret\
//                            << " - " << utils::code_to_string(ret));\
//                            return ret;\
//                        }\
//                    } while (false)

#define try_to(x)   do { auto ret = x;\
                        if (utils::exit_code(ret)) {\
                            if (utils::error_code(ret)) {\
                                log_error("Function " << (#x)\
                                << " failed with code: " << ret\
                                << " - " << utils::code_to_string(ret));\
                            }\
                            return ret;\
                        }\
                    } while (false)

/* ? */ //TODO throw fpp exception
#define try_throw(x)    do { auto ret = x;\
                            if (utils::exit_code(ret)) {\
                                log_error("Function " << (#x)\
                                << " failed with code: " << ret\
                                << " - " << utils::code_to_string(ret));\
                            }\
                        } while (false)

/* ? */
#define try_throw_static(x) do { auto ret = x;\
                                if (utils::exit_code(ret)) {\
                                    static_log_error("TODO", "Function " << (#x)\
                                    << " failed with code: " << ret\
                                    << " - " << utils::code_to_string(ret));\
                                }\
                            } while (false)

/* ? */ // В случае ошибки не возвращать ее код, а выбрасывать исключение (ex. в деструкторах); добавить параметр - текст ошибки
//#define try_and_throw(x,msg) { auto ret = x;\
//                    if (utils::exit_code(ret)) {\
//                        log_error("Function " << (#x)\
//                        << " failed with code: " << ret\
//                        << " - " << utils::code_to_string(ret));\
//                        return ret;\
//                    }\
//                  } SEMICOLON_REQUIREMENT

/* ? */
//TODO
// Возрат кода и отправка сообщения в лог, сответствующий коду

/* ? */
#define return_if(cond,ret_value)   do {\
                                        if (cond) {\
                                            return ret_value;\
                                        }\
                                    } while(false)

/* ? */
#define return_if_not(cond,ret_value)   do {\
                                            if (!(cond)) {\
                                                return ret_value;\
                                            }\
                                        } while (false)

/* ? */
#define return_info_if(cond,msg,ret_value)  do {\
                                                if (cond) {\
                                                    log_info(msg);\
                                                    return ret_value;\
                                                }\
                                            } while(false)

#define return_warning_if(cond,msg,ret_value)  do {\
                                                if (cond) {\
                                                    log_warning(msg);\
                                                    return ret_value;\
                                                }\
                                            } while(false)

//TODO объединить макросы в один, определять тип лога исходя из кода возврата 13.01
#define return_warning_if_not(cond,msg,ret_value)  do {\
                                                if_not(cond) {\
                                                    log_warning(msg);\
                                                    return ret_value;\
                                                }\
                                            } while(false)

#define return_error_if(cond,msg,ret_value)  do {\
                                                if (cond) {\
                                                    log_error(msg);\
                                                    return ret_value;\
                                                }\
                                            } while(false)

#define return_error_if_not(cond,msg,ret_value) do {\
                                                    if_not(cond) {\
                                                        log_error(msg);\
                                                        return ret_value;\
                                                    }\
                                                } while(false)

/* ? */
#define trow_if(cond,error_message) //TODO

/* ? */
#define if_not(x)       if(!(x))
#define while_not(x)    while(!(x))

/* Значения для задержек в коде в мс */
#define SHORT_DELAY_MS          10
#define MEDIUM_DELAY_MS         100
#define LONG_DELAY_MS           1000

/* ? */
//#define TAB                     "\t"
#define TAB                     "  "

/* ? */
#define EMPTY_CONSTRUCTOR
#define EMPTY_DESTRUCTOR

/* ? */
#define KILOBYTES * 1024
#define MEGABYTES * 1024 * 1024

/* ? */
#define BROADCAST -1
