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
    COPY_VIDEO,
    COPY_AUDIO
};
