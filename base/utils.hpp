#pragma once

#include "ffmpeg.hpp"
#include "YLogger.hpp"
//#include "YContext.hpp"
#include "YException.hpp"
//#include "../control/YVideoStream.hpp"
//#include "../control/YAudioStream.hpp"
#include "../base/YStream.hpp"
#include "../control/YVideoParameters.hpp"
#include "../control/YAudioParameters.hpp"
#include <string>
#include <sstream>
#include <algorithm>

static int64_t object_uid_handle = DEFAULT_INT; //TODO remove warning

typedef std::list<YObject*> ProcessorList;

class utils
{

public:

    utils()         = delete;
    ~utils()        = delete;
    utils(utils&)   = delete;
    utils(utils&&)  = delete;

    static std::string  media_type_to_string(YMediaType media_type);
    static std::string  pts_to_string(int64_t pts);
    static void         sleep_for(int64_t milliseconds);
    static bool         exit_code(YCode code);
    static std::string  code_to_string(YCode code);
    static std::string  rational_to_string(AVRational rational);
    static bool         compatibleWithSampleFormat(AVCodecContext* codec_context, AVSampleFormat sample_format);
    static AVMediaType  ymedia_type_to_avmedia_type(YMediaType media_type);
    static int64_t      gen_context_uid();
    static int64_t      gen_stream_uid(int64_t context_uid, int64_t stream_index);
    static int64_t      get_context_uid(int64_t stream_uid);
    static std::string  guess_format_short_name(std::string media_resurs_locator);
    static AVCodec*     find_codec(std::string codec_short_name);
    static AVCodec*     find_codec(AVCodecID codec_id);

    static YCode        init_codecpar(AVCodecParameters* codecpar, AVCodec* codec);
    static void         parameters_to_context(YParameters* parametres, AVCodecContext* codec);
    static void         parameters_from_context(YParameters* parametres, AVCodecContext* codec);
    static void         parameters_to_avcodecpar(YParameters* parametres, AVCodecParameters* codecpar);
    static void         parameters_from_avcodecpar(YParameters* parametres, AVCodecParameters* codecpar);

    static YVideoParameters*    default_video_parameters(AVCodecID codec_id);
    static YAudioParameters*    default_audio_parameters(AVCodecID codec_id);

    static bool         rescalingRequired(StreamPair streams);
    static bool         resamplingRequired(StreamPair streams);
    static bool         transcodingRequired(StreamPair streams);

    static YStream*     find_best_stream(const StreamVector& stream_list);

};

/* Макрос получения экзмепляра объекта класса YLogger */
#define logger                  YLogger::instance()

/* Макрос установки уровня лога; сообщения, имеющие урень выше установленного, игнорируются */
#define set_log_level(x)        logger.setLogLevel(x)
#define set_ffmpeg_log_level(x) logger.setFfmpegLogLevel(x)
#define stop_log()              logger.quit(); logger.join() //TODO не работает, вылет при завершении - поток лога не завершается

/* Макросы для отправки строковых сообщений в лог */
#define print_info(x)       logger.print(this, code_pos, YLogLevel::Info,    x)
#define print_warning(x)    logger.print(this, code_pos, YLogLevel::Warning, x)
#define print_error(x)      logger.print(this, code_pos, YLogLevel::Error,   x)
#define print_debug(x)      logger.print(this, code_pos, YLogLevel::Debug,   x)

/* Макросы для отправки потоковых сообщений в лог */
#define log_info(x)         { std::stringstream log_ss; log_ss << x; print_info(log_ss.str());      } SEMICOLON_REQUIREMENT
#define log_warning(x)      { std::stringstream log_ss; log_ss << x; print_warning(log_ss.str());   } SEMICOLON_REQUIREMENT
#define log_error(x)        { std::stringstream log_ss; log_ss << x; print_error(log_ss.str());     } SEMICOLON_REQUIREMENT
#define log_debug(x)        { std::stringstream log_ss; log_ss << x; print_debug(log_ss.str());     } SEMICOLON_REQUIREMENT

/* ? */
#define code_pos            __FUNCTION__
