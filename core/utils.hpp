#pragma once
#include "ffmpeg.hpp"
#include <core/Logger.hpp>
#include <core/Exception.hpp>
#include <stream.hpp>
#include <sstream>

namespace ffmpeg { extern "C" {
    #include <libavcodec/avcodec.h>
} } // namespace ffmpeg

namespace fpp {

    class utils {

    public:

        static uid_t        gen_uid();
        static uid_t        gen_stream_uid(uid_t context_uid, uid_t stream_index);
        static uid_t        get_context_uid(uid_t stream_uid);

        //TODO сделать перегрузку одного метода utils::to_string(...) 24.01
        static std::string  media_type_to_string(MediaType value);
        static std::string  pts_to_string(int64_t pts);
        static std::string  bool_to_string(bool value);
        static std::string  pixel_format_to_string(ffmpeg::AVPixelFormat value);
        static std::string  sample_format_to_string(ffmpeg::AVSampleFormat value);
        static std::string  time_to_string(int64_t time_stamp, ffmpeg::AVRational time_base);
        static std::string  code_to_string(Code value);
        static std::string  codec_type_to_string(CodecType type);
        static std::string  rational_to_string(ffmpeg::AVRational rational);

        static auto         makeVideoParams(SharedParameters params) -> SharedVideoParameters;
        static auto         makeAudioParams(SharedParameters params) -> SharedAudioParameters;

        static void         sleep_for(int64_t milliseconds);
        static void         sleep_for_ms(int64_t milliseconds);
        static void         sleep_for_sec(int64_t seconds);
        static void         sleep_for_min(int64_t minutes);

        static ffmpeg::AVCodec*     find_decoder(std::string codec_short_name);
        static ffmpeg::AVCodec*     find_encoder(std::string codec_short_name);
        static ffmpeg::AVCodec*     find_decoder(ffmpeg::AVCodecID codec_id);
        static ffmpeg::AVCodec*     find_encoder(ffmpeg::AVCodecID codec_id);

        static bool         rescaling_required(const IOParams params);
        static bool         resampling_required(const IOParams params);
        static bool         transcoding_required(const IOParams params);
        static bool         video_filter_required(const IOParams params);
        static bool         audio_filter_required(const IOParams params);

        static bool         compare_float(float a, float b);
        static bool         equal_rational(ffmpeg::AVRational a, ffmpeg::AVRational b);

        static bool         exit_code(Code value);
        static bool         error_code(Code value);

        static bool         compatible_with_pixel_format(const ffmpeg::AVCodec* codec, ffmpeg::AVPixelFormat pixel_format);
        static bool         compatible_with_sample_format(const ffmpeg::AVCodec* codec, ffmpeg::AVSampleFormat sample_format);

        static SharedStream find_best_stream(const StreamVector& stream_list);


        /* ---- R E F A C T O R I N G ---- */



        static SharedStream createInputStream();
        static SharedStream createOutputStream();


        static ffmpeg::AVMediaType  mediatype_to_avmediatype(MediaType media_type);
        static MediaType    avmt_to_mt(ffmpeg::AVMediaType avmedia_type);
        static std::string  guess_format_short_name(std::string media_resurs_locator);


        static SharedParameters createParams(MediaType type, ParamsType par_type);

        static Code         init_codecpar(ffmpeg::AVCodecParameters* codecpar, ffmpeg::AVCodec* codec);
        static void         parameters_to_context(const SharedParameters params, ffmpeg::AVCodecContext* codec_context); //TODO перенести внутрь кодекконтекста 17.01
        static void         parameters_to_avcodecpar(const SharedParameters params, ffmpeg::AVCodecParameters* codecpar);
        static void         parameters_from_avcodecpar(Parameters* parametres, ffmpeg::AVCodecParameters* codecpar);

        static MediaType    antitype(const MediaType value);

    };

    inline bool operator==(const ffmpeg::AVRational& lhs, const ffmpeg::AVRational& rhs) {
        return av_cmp_q(lhs, rhs) == 0;
    }

    inline bool operator!=(const ffmpeg::AVRational& lhs, const ffmpeg::AVRational& rhs) {
        return av_cmp_q(lhs, rhs) != 0;
    }

    inline std::ostream& operator<<(std::ostream& os, const ffmpeg::AVRational rational) {
        os << utils::rational_to_string(rational);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const ffmpeg::AVPixelFormat pix_fmt) {
        os << utils::pixel_format_to_string(pix_fmt);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const ffmpeg::AVSampleFormat smpl_fmt) {
        os << utils::sample_format_to_string(smpl_fmt);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const ffmpeg::AVCodecID codec_id) {
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



