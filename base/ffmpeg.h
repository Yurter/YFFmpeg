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

/* Варианты для быстрой преднастройки */
/* параметров медиа-ресурсa           */
enum YMediaPreset {
    Auto,
    /* Input */
    Silence,
    /* Output */
    YouTube,
    Timelapse
};

/* Коды результата выполнения некоторых функций */
enum YCode {
    END_OF_FILE,
    AGAIN
};

/* Опции для настройки  */
enum YOptions {
    COPY_VIDEO      = 0x0001,
    COPY_AUDIO      = 0x0002,
    DUMMY_ENUM      = 0x0004
};

/* ? */
#define INVALID_INT             -1
#define DEFAULT_INT             0
#define DEFAULT_FLOAT           0.f
#define DEFAULT_STRING          "none"
#define DEFAULT_RATIONAL        { 0, 1 }
#define DEFAULT_CODEC_ID        AV_CODEC_ID_NONE
#define DEFAULT_PIXEL_FORMAT    AV_PIX_FMT_NONE
#define DEFAULT_SAMPLE_FORMAT   AV_SAMPLE_FMT_NONE
#define DEFAULT_CHANEL_LAYOUT   0

#define invalid_int(x)          ((x) == INVALID_INT)
#define not_inited_int(x)       ((x) == DEFAULT_INT)
#define not_inited_float(x)     ((x) == DEFAULT_FLOAT)
#define not_inited_string(x)    ((x) == DEFAULT_STRING)
#define not_inited_q(x)         (av_cmp_q(x, DEFAULT_RATIONAL) == 0)
#define not_inited_codec_id(x)  ((x) == DEFAULT_CODEC_ID)
#define not_inited_pix_fmt(x)   ((x) == DEFAULT_PIXEL_FORMAT)
#define not_inited_smp_fmt(x)   ((x) == DEFAULT_SAMPLE_FORMAT)
#define not_inited_ch_layout(x) ((x) == DEFAULT_CHANEL_LAYOUT)
