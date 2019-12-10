#pragma once
#include "ffmpeg.hpp"
#include "Logger.hpp"
#include "Exception.hpp"
#include "base/Stream.hpp"
#include "stream/VideoParameters.hpp"
#include "stream/AudioParameters.hpp"
#include <string>
#include <sstream>
#include <algorithm>

namespace fpp {

    static int64_t object_uid_handle = DEFAULT_INT; //TODO remove warning
    #define USE_HWACCEL true //TODO

    class utils {

    public:

        utils()         = delete;
        utils(utils&)   = delete;
        utils(utils&&)  = delete;

        static std::string  media_type_to_string(MediaType media_type);
        static std::string  pts_to_string(int64_t pts);
        static std::string  bool_to_string(bool value);

        static void         sleep_for(int64_t milliseconds);
        static void         sleep_for_ms(int64_t milliseconds);
        static void         sleep_for_sec(int64_t seconds);
        static void         sleep_for_min(int64_t minutes);

        static bool         exit_code(Code code);
        static bool         error_code(Code code);
        static std::string  code_to_string(Code code);
        static std::string  codec_type_to_string(CodecType type);
        static std::string  rational_to_string(AVRational rational);
        static bool         compatible_with_pixel_format(AVCodec* codec, AVPixelFormat pixel_format);
        static bool         compatibleWithSampleFormat(AVCodecContext* codec_context, AVSampleFormat sample_format);
        static AVMediaType  ymedia_type_to_avmedia_type(MediaType media_type);
        static int64_t      gen_uid();
        static int64_t      gen_stream_uid(int64_t context_uid, int64_t stream_index);
        static int64_t      get_context_uid(int64_t stream_uid);
        static std::string  guess_format_short_name(std::string media_resurs_locator);
        static AVCodec*     find_decoder(std::string codec_short_name);
        static AVCodec*     find_decoder(AVCodecID codec_id);
        static AVCodec*     find_encoder(std::string codec_short_name);
        static AVCodec*     find_encoder(AVCodecID codec_id);

        static Code         init_codecpar(AVCodecParameters* codecpar, AVCodec* codec);
        static void         parameters_to_context(const Parameters * const param, AVCodecContext* codec);
//        static void         parameters_from_context(Parameters* parametres, AVCodecContext* codec);
        static void         parameters_to_avcodecpar(Parameters* parametres, AVCodecParameters* codecpar);
        static void         parameters_from_avcodecpar(Parameters* parametres, AVCodecParameters* codecpar);

        static VideoParameters*    default_video_parameters(AVCodecID codec_id);
        static AudioParameters*    default_audio_parameters(AVCodecID codec_id);

        static Code         find_encoder_for(const Parameters * const src_prm, Parameters * const dst_prm/*, bool use_hwaccel*/);

        static bool         rescaling_required(const IOParams params);
        static bool         resampling_required(const IOParams params);
        static bool         video_filter_required(const IOParams params);
        static bool         audio_filter_required(const IOParams params);
        static bool         transcoding_required(const IOParams params);

        static Stream*      find_best_stream(const StreamVector& stream_list);

        static bool         compare_float(float a, float b);

        static int          save_frame_as_jpeg(AVCodecContext* pCodecCtx, AVFrame* pFrame, int FrameNo);
        static void SaveAvFrame(AVFrame *avFrame);

//        static cv::Mat      frame_to_mat(Frame)

    };

} // namespace fpp

/* Макрос получения экзмепляра объекта класса Logger */
#define logger fpp::Logger::instance("fpp_log")

/* Макрос установки дирректории, в которой находятся файлы лога.
 * Должен вызыватся первым по отношению к остальным макросам    */
#define set_log_dir(x) fpp::Logger::instance(x)

/* Макрос установки уровня лога - сообщения, имеющие урень выше установленного, игнорируются */
#define set_log_level(x)        logger.setLogLevel(x)
#define set_ffmpeg_log_level(x) logger.setFFmpegLogLevel(x)

/* Макросы для отправки строковых сообщений в лог */
#define print_info(caller_name,x)       logger.print(caller_name, code_pos, LogLevel::Info,    x)
#define print_warning(caller_name,x)    logger.print(caller_name, code_pos, LogLevel::Warning, x)
#define print_error(caller_name,x)      logger.print(caller_name, code_pos, LogLevel::Error,   x)
#define print_debug(caller_name,x)      logger.print(caller_name, code_pos, LogLevel::Debug,   x)
#define print_trace(caller_name,x)      logger.print(caller_name, code_pos, LogLevel::Trace,   x)

//#define print_msg(caller_name,lvl,msg)  logger.print(caller_name, code_pos, lvl, msg)
//#define log_info(x)     NSFPP( if_not(logger.ignoreMessage(LogLevel::Info))     { std::stringstream log_ss; log_ss << x; print_msg(this->name(),LogLevel::Info,log_ss.str());     })

/* ? */
#define NSFPP(x) do { using namespace fpp; x } while (false)

/* Макросы для отправки потоковых сообщений в лог */
#define log_info(x)     NSFPP( if_not(logger.ignoreMessage(LogLevel::Info))     { std::stringstream log_ss; log_ss << x; print_info(this->name(),log_ss.str());     })
#define log_warning(x)  NSFPP( if_not(logger.ignoreMessage(LogLevel::Warning))  { std::stringstream log_ss; log_ss << x; print_warning(this->name(),log_ss.str());  })
#define log_error(x)    NSFPP( if_not(logger.ignoreMessage(LogLevel::Error))    { std::stringstream log_ss; log_ss << x; print_error(this->name(),log_ss.str());    })
#define log_debug(x)    NSFPP( if_not(logger.ignoreMessage(LogLevel::Debug))    { std::stringstream log_ss; log_ss << x; print_debug(this->name(),log_ss.str());    })
#define log_trace(x)    NSFPP( if_not(logger.ignoreMessage(LogLevel::Trace))    { std::stringstream log_ss; log_ss << x; print_trace(this->name(),log_ss.str());    })

/* Макросы для отправки потоковых сообщений в лог вне контекста fpp */
#define static_log_info(caller_name,x)      NSFPP( if_not(logger.ignoreMessage(LogLevel::Info))     { std::stringstream log_ss; log_ss << x; print_info(caller_name,log_ss.str());   })
#define static_log_warning(caller_name,x)   NSFPP( if_not(logger.ignoreMessage(LogLevel::Warning))  { std::stringstream log_ss; log_ss << x; print_warning(caller_name,log_ss.str());})
#define static_log_error(caller_name,x)     NSFPP( if_not(logger.ignoreMessage(LogLevel::Error))    { std::stringstream log_ss; log_ss << x; print_error(caller_name,log_ss.str());  })
#define static_log_debug(caller_name,x)     NSFPP( if_not(logger.ignoreMessage(LogLevel::Debug))    { std::stringstream log_ss; log_ss << x; print_debug(caller_name,log_ss.str());  })
#define static_log_trace(caller_name,x)     NSFPP( if_not(logger.ignoreMessage(LogLevel::Trace))    { std::stringstream log_ss; log_ss << x; print_trace(caller_name,log_ss.str());  })
#define static_log_auto(caller_name,lvl,x)  NSFPP( if_not(logger.ignoreMessage(lvl))    { std::stringstream log_ss; log_ss << x; print_trace(caller_name,log_ss.str());  })

/* ? */
#define code_pos std::string(__FUNCTION__) + ", line: " + std::to_string(__LINE__)
//#define current_time
