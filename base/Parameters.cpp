#include "Parameters.hpp"
#include "core/utils.hpp"

namespace fpp {

    Parameters::Parameters(MediaType type) :
        MediaData(type),
        _codec(nullptr)
        , _codec_id(DEFAULT_CODEC_ID)
        , _codec_name(DEFAULT_STRING)
        , _codec_type(CodecType::Unknown)
        , _bitrate(DEFAULT_INT)
        , _duration(DEFAULT_INT)
        , _stream_index(INVALID_INT)
        , _stream_uid(INVALID_INT)
        , _time_base(DEFAULT_RATIONAL)
        , _context_uid(INVALID_INT)
    {
        setName("Parameters");
    }

    void Parameters::setCodec(AVCodecID codec_id, CodecType codec_type) {
        switch (codec_type) {
        case CodecType::Unknown:
            log_error("Unknown codec type of " << avcodec_get_name(codec_id) << ", ignored");
            return;
        case CodecType::Decoder:
            setCodecType(codec_type);
            setCodec(utils::find_decoder(codec_id));
            return;
        case CodecType::Encoder:
            setCodecType(codec_type);
            setCodec(utils::find_encoder(codec_id));
            return;
        }
    }

    void Parameters::setCodec(std::string codec_short_name, CodecType codec_type) {
        switch (codec_type) {
        case CodecType::Unknown:
            log_error("Unknown codec type of " << codec_short_name << ", ignored");
            return;
        case CodecType::Decoder:
            setCodecType(codec_type);
            setCodec(utils::find_decoder(codec_short_name));
            return;
        case CodecType::Encoder:
            setCodecType(codec_type);
            setCodec(utils::find_encoder(codec_short_name));
            return;
        }
    }

    void Parameters::setCodec(AVCodec* codec) {
        if (inited_ptr(codec)) {
            _codec = codec;
            _codec_id = codec->id;
            _codec_name = codec->name;
        } else {
            log_error("Failed codec search");
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

    void Parameters::setStreamIndex(int64_t stream_index) {
        _stream_index = stream_index;
        setStreamUid(fpp::utils::gen_stream_uid(_context_uid, _stream_index));//TODO
    }

    void Parameters::setStreamUid(int64_t value) {
        _stream_uid = value;
    }

    void Parameters::setTimeBase(AVRational time_base) {
        _time_base = time_base;
    }

    void Parameters::setContextUid(int64_t context_uid) {
        if_not(invalid_int(_context_uid)) {
            log_warning("Cannot set context_uid again: " << context_uid << ", ignored");
            return;
        }
        _context_uid = context_uid;
        setStreamUid(fpp::utils::gen_stream_uid(_context_uid, _stream_index));//TODO
    }

    AVCodecID Parameters::codecId() const {
        return _codec_id;
    }

    std::string Parameters::codecName() const {
        return _codec_name;
    }

    AVCodec* Parameters::codec() const {
        return _codec;
    }

    CodecType Parameters::codecType() const {
        return _codec_type;
    }

    int64_t Parameters::bitrate() const {
        return _bitrate;
    }

    int64_t Parameters::duration() const {
        return _duration;
    }

    int64_t Parameters::streamIndex() const {
        return _stream_index;
    }

    int64_t Parameters::streamUid() const {
        return _stream_uid;
    }

    AVRational Parameters::timeBase() const {
        return _time_base;
    }

    int64_t Parameters::contextUid() const {
        return _context_uid;
    }

    void Parameters::increaseDuration(int64_t value) {
        _duration += value;
    }

    std::string Parameters::toString() const {
        std::string str;
        str += codecName() + ", ";
        str += utils::codec_type_to_string(codecType()) + ", ";
        str += "bit/s " + std::to_string(bitrate()) + ", ";
        str += "dur " + std::to_string(duration()) + ", ";
        str += "tb " + utils::rational_to_string(timeBase());
        return str;
    }

    Code Parameters::completeFrom(const Parameters* other_parametrs) {
        if (not_inited_codec_id(_codec_id)) {
            try_to(utils::find_encoder_for(other_parametrs, this));
        }

        if (not_inited_int(_bitrate)) { setBitrate(other_parametrs->bitrate());     }
        if (not_inited_q(_time_base)) { setTimeBase(other_parametrs->timeBase());   }
        return Code::OK;
    }

    void Parameters::setCodecType(CodecType value) {
        _codec_type = value;
    }

} // namespace fpp
