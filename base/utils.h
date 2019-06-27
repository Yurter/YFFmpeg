#pragma once

#include "ffmpeg.h"
#include "YLogger.h"
#include "../control/YVideoParameters.h"
#include "../control/YAudioParameters.h"
#include <string>
#include <sstream>

static int64_t object_uid_handle = DEFAULT_INT; //TODO warning

class utils
{

public:

    static std::string  media_type_to_string(YMediaType media_type);
    static void         sleep_for(int64_t milliseconds);
    static bool         exit_code(YCode code);
    static std::string  code_to_string(YCode code);
    static std::string  rational_to_string(AVRational rational);
    static bool         compatibleWithSampleFormat(AVCodecContext* codec_context, AVSampleFormat sample_format);
    static AVMediaType  ymedia_type_to_avmedia_type(YMediaType media_type);
    static int64_t      gen_context_uid();
    static int64_t      gen_stream_uid(int64_t context_uid, int64_t stream_index);

};

/* ? */
#define logger              YLogger::instance()

/* Макрос установки уровня лога, сообщения имеющие урень выше установленного игнорируются */
#define set_log_level(x)    logger.setLogLevel(x)
#define stop_log()          logger.quit(); logger.join() //TODO не работает, вылет при завершении поток логера не завершается

/* Макросы для отправки строковых сообщений в лог */
#define print_info(x)       logger.print(this, YLogLevel::Info, x)
#define print_warning(x)    logger.print(this, YLogLevel::Warning, x)
#define print_error(x)      logger.print(this, YLogLevel::Error, x)
#define print_debug(x)      logger.print(this, YLogLevel::Debug, x)

/* Макросы для отправки потоковых сообщений в лог */
#define log_info(x)         { std::stringstream log_ss; log_ss << x; print_info(log_ss.str());      }
#define log_warning(x)      { std::stringstream log_ss; log_ss << x; print_warning(log_ss.str());   }
#define log_error(x)        { std::stringstream log_ss; log_ss << x; print_error(log_ss.str());     }
#define log_debug(x)        { std::stringstream log_ss; log_ss << x; print_debug(log_ss.str());     }
