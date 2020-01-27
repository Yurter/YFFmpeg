#pragma once
#include <fpp/core/ffmpeg.hpp>
#include <fpp/core/Logger.hpp>
#include <fpp/core/FFmpegException.hpp>
#include <fpp/streamHeader.hpp>
#include <sstream>

extern "C" {
    #include <libavcodec/avcodec.h>
}

namespace fpp {

    class utils {

    public:

        static uid_t        gen_uid();
        static uid_t        gen_stream_uid(uid_t context_uid, uid_t stream_index);
        static uid_t        get_context_uid(uid_t stream_uid);

        //TODO сделать перегрузку одного метода utils::to_string(...) 24.01
        static std::string  to_string(MediaType value);
        static std::string  pts_to_string(int64_t pts);
        static std::string  to_string(bool value);
        static std::string  to_string(AVPixelFormat value);
        static std::string  to_string(AVSampleFormat value);
        static std::string  to_string(Code value);
        static std::string  to_string(CodecType type);
        static std::string  to_string(AVRational rational);
        static std::string  time_to_string(int64_t time_stamp, AVRational time_base);

        static auto         makeVideoParams(SharedParameters params) -> SharedVideoParameters;
        static auto         makeAudioParams(SharedParameters params) -> SharedAudioParameters;

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

        static SharedStream find_best_stream(const StreamVector& stream_list);

        static const char*  guess_format_short_name(std::string media_resurs_locator);

        /* ---- R E F A C T O R I N G ---- */



        static SharedStream createInputStream();
        static SharedStream createOutputStream();


        static AVMediaType  mediatype_to_avmediatype(MediaType media_type);
        static MediaType    avmt_to_mt(AVMediaType avmedia_type);


        static SharedParameters createParams(MediaType type);

        static Code         init_codecpar(AVCodecParameters* codecpar, AVCodec* codec);
        static void         parameters_to_context(const SharedParameters params, AVCodecContext* codec_context); //TODO перенести внутрь кодекконтекста 17.01
        static void         parameters_to_avcodecpar(const SharedParameters params, AVCodecParameters* codecpar);
        static void         parameters_from_avcodecpar(Parameters* parametres, AVCodecParameters* codecpar);

        static MediaType    antitype(const MediaType value);

    };

    inline bool operator==(const AVRational& lhs, const AVRational& rhs) {
        return av_cmp_q(lhs, rhs) == 0;
    }

    inline bool operator!=(const AVRational& lhs, const AVRational& rhs) {
        return av_cmp_q(lhs, rhs) != 0;
    }

    inline std::ostream& operator<<(std::ostream& os, const AVRational rational) {
        os << utils::to_string(rational);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const AVPixelFormat pix_fmt) {
        os << utils::to_string(pix_fmt);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const AVSampleFormat smpl_fmt) {
        os << utils::to_string(smpl_fmt);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const AVCodecID codec_id) {
        os << avcodec_get_name(codec_id);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const MediaType type) {
        os << utils::to_string(type);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const Code code) {
        os << utils::to_string(code);
        return os;
    }

} // namespace fpp



