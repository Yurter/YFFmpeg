#pragma once

#include "ffmpeg.h"
#include "Logger.h"
#include "YParameters.h"
#include <string>
#include <sstream>

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
    static int64_t      gen_stream_uid(int64_t context_index, int64_t stream_index);
    static void         codecpar_from_parameters(AVCodecParameters* codecpar, YParameters* parameters);

};

/* Макрос установки уровня лога, сообщения имеющие урень выше установленного игнорируются */
#define set_log_level(x)    Logger::instance().setLogLevel(x)

/* Макросы для отправки строковых сообщений в лог */
#define print_info(x)       Logger::instance().print(this, YLogLevel::Info, x)
#define print_warning(x)    Logger::instance().print(this, YLogLevel::Warning, x)
#define print_error(x)      Logger::instance().print(this, YLogLevel::Error, x)
#define print_debug(x)      Logger::instance().print(this, YLogLevel::Debug, x)

/* Макросы для отправки потоковых сообщений в лог */
#define log_info(x)         { std::stringstream log_ss; log_ss << x; print_info(log_ss.str());      }
#define log_warning(x)      { std::stringstream log_ss; log_ss << x; print_warning(log_ss.str());   }
#define log_error(x)        { std::stringstream log_ss; log_ss << x; print_error(log_ss.str());     }
#define log_debug(x)        { std::stringstream log_ss; log_ss << x; print_debug(log_ss.str());     }
