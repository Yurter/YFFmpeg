#pragma once

#include "ffmpeg.hpp"
#include "Logger.hpp"
#include "Exception.hpp"
#include "../base/Stream.hpp"
#include "../control/VideoParameters.hpp"
#include "../control/AudioParameters.hpp"
#include <string>
#include <sstream>
#include <algorithm>

namespace fpp {

    static int64_t object_uid_handle = DEFAULT_INT; //TODO remove warning

    //typedef std::list<Object*> ProcessorList;
    using ProcessorList = std::list<Object*>;

    class utils {

    public:

        utils()         = delete;
        ~utils()        = delete;
        utils(utils&)   = delete;
        utils(utils&&)  = delete;

        static std::string  media_type_to_string(MediaType media_type);
        static std::string  pts_to_string(int64_t pts);
        static void         sleep_for(int64_t milliseconds);
        static bool         exit_code(Code code);
        static bool         error_code(Code code);
        static std::string  code_to_string(Code code);
        static std::string  rational_to_string(AVRational rational);
        static bool         compatibleWithSampleFormat(AVCodecContext* codec_context, AVSampleFormat sample_format);
        static AVMediaType  ymedia_type_to_avmedia_type(MediaType media_type);
        static int64_t      gen_context_uid();
        static int64_t      gen_stream_uid(int64_t context_uid, int64_t stream_index);
        static int64_t      get_context_uid(int64_t stream_uid);
        static std::string  guess_format_short_name(std::string media_resurs_locator);
        static AVCodec*     find_decoder(std::string codec_short_name);
        static AVCodec*     find_decoder(AVCodecID codec_id);
        static AVCodec*     find_encoder(std::string codec_short_name);
        static AVCodec*     find_encoder(AVCodecID codec_id);

        static Code         init_codecpar(AVCodecParameters* codecpar, AVCodec* codec);
        static void         parameters_to_context(Parameters* parametres, AVCodecContext* codec);
//        static void         parameters_from_context(Parameters* parametres, AVCodecContext* codec);
        static void         parameters_to_avcodecpar(Parameters* parametres, AVCodecParameters* codecpar);
        static void         parameters_from_avcodecpar(Parameters* parametres, AVCodecParameters* codecpar);

        static VideoParameters*    default_video_parameters(AVCodecID codec_id);
        static AudioParameters*    default_audio_parameters(AVCodecID codec_id);

        static bool         rescaling_required(const StreamPair streams);
        static bool         resampling_required(const StreamPair streams);
        static bool         video_filter_required(const StreamPair streams);
        static bool         audio_filter_required(const StreamPair streams);
        static bool         transcoding_required(const StreamPair streams);

        static Stream*      find_best_stream(const StreamVector& stream_list);

        static bool         compareFloat(float a, float b);

        static int          save_frame_as_jpeg(AVCodecContext *pCodecCtx, AVFrame *pFrame, int FrameNo);

    };

} // namespace fpp

/* Макрос получения экзмепляра объекта класса Logger */
#define logger                  Logger::instance()

/* Макрос установки уровня лога; сообщения, имеющие урень выше установленного, игнорируются */
#define set_log_level(x)        logger.setLogLevel(x)
#define set_ffmpeg_log_level(x) logger.setFfmpegLogLevel(x)
#define stop_log()              logger.quit()

/* Макросы для отправки строковых сообщений в лог */
#define print_info(x)       logger.print(this, code_pos, LogLevel::Info,    x)
#define print_warning(x)    logger.print(this, code_pos, LogLevel::Warning, x)
#define print_error(x)      logger.print(this, code_pos, LogLevel::Error,   x)
#define print_debug(x)      logger.print(this, code_pos, LogLevel::Debug,   x)

#define static_print_info(caller_name,msg)      logger.static_print(caller_name, code_pos, LogLevel::Info,    msg)
#define static_print_warning(caller_name,msg)   logger.static_print(caller_name, code_pos, LogLevel::Warning, msg)
#define static_print_error(caller_name,msg)     logger.static_print(caller_name, code_pos, LogLevel::Error,   msg)
#define static_print_debug(caller_name,msg)     logger.static_print(caller_name, code_pos, LogLevel::Debug,   msg)
#define static_print_auto(caller_name,lvl,msg)  logger.static_print(caller_name, code_pos, static_cast<fpp::LogLevel>(lvl), msg)

/* Макросы для отправки потоковых сообщений в лог */
#define log_info(x)         { std::stringstream log_ss; log_ss << x; print_info(log_ss.str());      } SEMICOLON_REQUIREMENT
#define log_warning(x)      { std::stringstream log_ss; log_ss << x; print_warning(log_ss.str());   } SEMICOLON_REQUIREMENT
#define log_error(x)        { std::stringstream log_ss; log_ss << x; print_error(log_ss.str());     } SEMICOLON_REQUIREMENT
#define log_debug(x)        { std::stringstream log_ss; log_ss << x; print_debug(log_ss.str());     } SEMICOLON_REQUIREMENT

/* ? */
#define code_pos            std::string(__FUNCTION__) + ", line: " + std::to_string(__LINE__)
