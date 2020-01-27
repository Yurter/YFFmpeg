#include "Parameters.hpp"
#include <fpp/core/Utils.hpp>

extern "C" {
    #include <libavformat/avformat.h>
}

#define DEFAULT_CODEC_ID        AV_CODEC_ID_NONE
#define inited_codec_id(x)      ((x) != DEFAULT_CODEC_ID)
#define not_inited_codec_id(x)  ((x) == DEFAULT_CODEC_ID)

namespace fpp {

    Parameters::Parameters()
        : _codec { nullptr }
        , _bitrate { 0 }
        , _duration { 0 }
        , _stream_index { INVALID_INT }
        , _stream_uid { INVALID_INT }
        , _time_base { DEFAULT_RATIONAL }
        , _context_uid { INVALID_INT } {
        setName("Parameters");
    }

    void Parameters::setDecoder(AVCodecID codec_id) {
        setCodec(avcodec_find_encoder(codec_id));
    }

    void Parameters::setEncoder(AVCodecID codec_id) {
        setCodec(avcodec_find_decoder(codec_id));
    }

    void Parameters::setCodec(AVCodec* codec) {
        if (inited_ptr(codec)) {
            _codec = codec;
        } else {
            log_error("Cannot set nullptr as a codec");
        }
    }

    void Parameters::setBitrate(int64_t bitrate) {
        _bitrate = bitrate;
    }

    void Parameters::setDuration(int64_t duration) {
        if (duration < 0) {
            log_warning("Cannot set duration less then zero: " << duration << ", ignored");
            return;
        }
        if (duration > LONG_MAX) {
            log_warning("Cannot set duration more then LONG_MAX, ignored");
            return;
        }
        _duration = duration;
    }

    void Parameters::setStreamIndex(uid_t stream_index) {
        _stream_index = stream_index;
        setStreamUid(fpp::utils::gen_stream_uid(_context_uid, _stream_index));//TODO
    }

    void Parameters::setStreamUid(uid_t value) {
        _stream_uid = value;
    }

    void Parameters::setTimeBase(AVRational time_base) {
        _time_base = time_base;
    }

    void Parameters::setContextUid(uid_t context_uid) {
        if_not(invalid_int(_context_uid)) {
            log_warning("Cannot set context_uid again: " << context_uid << ", ignored");
            return;
        }
        _context_uid = context_uid;
        setStreamUid(fpp::utils::gen_stream_uid(_context_uid, _stream_index));//TODO
    }

    AVCodecID Parameters::codecId() const {
        return _codec->id;
    }

    std::string Parameters::codecName() const {
        return _codec->name;
    }

    AVCodec* Parameters::codec() const {
        return _codec;
    }

    int64_t Parameters::bitrate() const {
        return _bitrate;
    }

    int64_t Parameters::duration() const {
         return _duration;
    }

    uid_t Parameters::streamIndex() const {
        return _stream_index;
    }

    uid_t Parameters::streamUid() const {
        return _stream_uid;
    }

    AVRational Parameters::timeBase() const {
        return _time_base;
    }

    int64_t Parameters::contextUid() const {
        return _context_uid;
    }

    void Parameters::increaseDuration(const int64_t value) {
        _duration += value;
    }

    std::string Parameters::toString() const {
        return codecName() + ", "
//        str += utils::codec_type_to_string(codecType()) + ", ";
        + "bit/s " + std::to_string(bitrate()) + ", "
        + "dur " + std::to_string(duration()) + ", "
        + "tb " + utils::to_string(timeBase());
    }

    void Parameters::completeFrom(const SharedParameters other_params) {
        if (not_inited_codec_id(codecId())) { setEncoder(other_params->codecId());      }
        if (not_inited_int(bitrate()))      { setBitrate(other_params->bitrate());      }
        if (not_inited_q(timeBase()))       { setTimeBase(other_params->timeBase());    }
    }

    void Parameters::parseStream(const AVStream* avstream) {
        setDecoder(avstream->codecpar->codec_id);
        setBitrate(avstream->codecpar->bit_rate);
        setDuration(avstream->duration);
        setStreamIndex(avstream->index);
        setTimeBase(avstream->time_base);
    }

    void Parameters::initStream(AVStream* avstream) const {
        avstream->codecpar->codec_id = codecId();
        avstream->codecpar->bit_rate = bitrate();
        avstream->duration = duration();
        avstream->time_base = timeBase();
    }

    bool Parameters::betterThen(const SharedParameters& other) {
        return this->bitrate() > other->bitrate();
    }

} // namespace fpp
