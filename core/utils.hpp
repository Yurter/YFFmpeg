#pragma once
#include "ffmpeg.hpp"
#include "Logger.hpp"
#include "Exception.hpp"
#include "stream/Stream.hpp"
#include "stream/VideoParameters.hpp"
#include "stream/AudioParameters.hpp"
#include <sstream>
#include <algorithm>

namespace fpp {

    class utils {

    public:

        static UID          gen_uid();
        static UID          gen_stream_uid(UID context_uid, UID stream_index);
        static UID          get_context_uid(UID stream_uid);

        static std::string  media_type_to_string(MediaType value);
        static std::string  pts_to_string(int64_t pts);
        static std::string  bool_to_string(bool value);
        static std::string  pixel_format_to_string(AVPixelFormat value);
        static std::string  sample_format_to_string(AVSampleFormat value);
        static std::string  time_to_string(int64_t time_stamp, AVRational time_base);
        static std::string  code_to_string(Code value);
        static std::string  codec_type_to_string(CodecType type);
        static std::string  rational_to_string(AVRational rational);

        static void         sleep_for(int64_t milliseconds);
        static void         sleep_for_ms(int64_t milliseconds);
        static void         sleep_for_sec(int64_t seconds);
        static void         sleep_for_min(int64_t minutes);

        static AVCodec*     find_decoder(std::string codec_short_name);
        static AVCodec*     find_encoder(std::string codec_short_name);
        static AVCodec*     find_decoder(AVCodecID codec_id);
        static AVCodec*     find_encoder(AVCodecID codec_id);

        static bool         rescaling_required(const IOParams params);
        static bool         resampling_required(const IOParams params);
        static bool         transcoding_required(const IOParams params);
        static bool         video_filter_required(const IOParams params);
        static bool         audio_filter_required(const IOParams params);

        static bool         compare_float(float a, float b);
        static bool         equal_rational(AVRational a, AVRational b);

        static bool         exit_code(Code value);
        static bool         error_code(Code value);

        static bool         compatible_with_pixel_format(const AVCodec* codec, AVPixelFormat pixel_format);
        static bool         compatible_with_sample_format(const AVCodec* codec, AVSampleFormat sample_format);

        static StreamPointer    find_best_stream(const StreamVector& stream_list);


        /* ---- R E F A C T O R I N G ---- */


        static AVMediaType  mediatype_to_avmediatype(MediaType media_type);
        static MediaType    avmt_to_mt(AVMediaType avmedia_type);
        static std::string  guess_format_short_name(std::string media_resurs_locator);


        static ParametersPointer createParams(MediaType type, ParamsType par_type);

        static Code         init_codecpar(AVCodecParameters* codecpar, AVCodec* codec);
        static void         parameters_to_context(const ParametersPointer params, AVCodecContext* codec_context);
        static void         parameters_to_avcodecpar(const ParametersPointer params, AVCodecParameters* codecpar);
        static void         parameters_from_avcodecpar(Parameters* parametres, AVCodecParameters* codecpar);

//        static VideoParameters*    default_video_parameters(AVCodecID codec_id);
//        static AudioParameters*    default_audio_parameters(AVCodecID codec_id);

        static Code         find_encoder_for(const ParametersPointer src_prm, ParametersPointer dst_prm);

        static MediaType    antitype(const MediaType value);




        static int          save_frame_as_jpeg(AVCodecContext* pCodecCtx, AVFrame* pFrame, int FrameNo);
        static void SaveAvFrame(AVFrame *avFrame);

//        static cv::Mat      frame_to_mat(Frame)

    };

    inline bool operator==(const AVRational& lhs, const AVRational& rhs) {
        return av_cmp_q(lhs, rhs) == 0;
    }

    inline bool operator!=(const AVRational& lhs, const AVRational& rhs) {
        return av_cmp_q(lhs, rhs) != 0;
    }

    inline std::ostream& operator<<(std::ostream& os, const AVRational rational) {
        os << utils::rational_to_string(rational);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const AVPixelFormat pix_fmt) {
        os << utils::pixel_format_to_string(pix_fmt);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const AVSampleFormat smpl_fmt) {
        os << utils::sample_format_to_string(smpl_fmt);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const AVCodecID codec_id) {
        os << avcodec_get_name(codec_id);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const MediaType type) {
        os << utils::media_type_to_string(type);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const Code code) {
        os << utils::code_to_string(code);
        return os;
    }

} // namespace fpp

/* Обертка пространства имён fpp */
#define FPP_BEGIN do { using namespace fpp;
#define FPP_END } while (false)

/* Макрос получения экзмепляра объекта класса Logger */
#define logger fpp::Logger::instance("fpp_log")

/* Макрос установки дирректории, в которой находятся файлы лога.
 * Должен вызыватся первым по отношению к остальным макросам    */
#define set_log_dir(x) fpp::Logger::instance(x)

/* Макрос установки уровня лога - сообщения, имеющие урень выше установленного, игнорируются */
#define set_log_level(x)        logger.setLogLevel(x)
#define set_ffmpeg_log_level(x) logger.setFFmpegLogLevel(x)

/* Макрос для отправки строкового сообщения в лог */
//#define log_message(caller_name, log_level, message)    FPP_BEGIN\
//                                                            if constexpr (!logger.ignoreMessage(log_level)) {\
//                                                                std::stringstream log_ss;\
//                                                                log_ss << message;\
//                                                                logger.print(caller_name, code_pos, log_level, log_ss.str());\
//                                                            }\
//                                                        FPP_END
#define log_message(caller_name, log_level, message) FPP_BEGIN\
    if_not(logger.ignoreMessage(log_level)) {\
        std::stringstream log_ss;\
        log_ss << message;\
        logger.print(caller_name, CODE_POS, log_level, log_ss.str());\
    }\
    FPP_END

/* Макросы для отправки строкоых сообщений в лог */
#define log_info(message)       log_message(this->name(), LogLevel::Info,       message)
#define log_warning(message)    log_message(this->name(), LogLevel::Warning,    message)
#define log_error(message)      log_message(this->name(), LogLevel::Error,      message)
#define log_debug(message)      log_message(this->name(), LogLevel::Debug,      message)
#define log_verbose(message)    log_message(this->name(), LogLevel::Verbose,    message)
#define log_trace(message)      log_message(this->name(), LogLevel::Trace,      message)

/* Макросы для отправки потоковых сообщений в лог вне контекста fpp */
#define static_log_info(caller_name, message)       log_message(caller_name, LogLevel::Info,    message)
#define static_log_warning(caller_name, message)    log_message(caller_name, LogLevel::Warning, message)
#define static_log_error(caller_name, message)      log_message(caller_name, LogLevel::Error,   message)
#define static_log_debug(caller_name, message)      log_message(caller_name, LogLevel::Debug,   message)
#define static_log_trace(caller_name, message)      log_message(caller_name, LogLevel::Trace,   message)
#define static_log_verbose(caller_name, message)    log_message(caller_name, LogLevel::Verbose, message)
#define static_log(caller_name, log_level, message) log_message(caller_name, log_level,         message)

/* ? */
#define CODE_POS std::string(__FUNCTION__) + ", line: " + std::to_string(__LINE__)
//#define current_time

