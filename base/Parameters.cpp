#include "Parameters.hpp"
#include "core/utils.hpp"

namespace fpp {

    Parameters::Parameters(ParamsType io_type)
        : _codec { nullptr }
        , _codec_id { DEFAULT_CODEC_ID }
        , _codec_name { DEFAULT_STRING }
        , _io_type { io_type }
        , _bitrate { 0 }
        , _duration { 0 }
        , _stream_index { INVALID_INT }
        , _stream_uid { INVALID_INT }
        , _time_base { DEFAULT_RATIONAL }
        , _context_uid { INVALID_INT } {
        setName("Parameters");
    }

    void Parameters::setCodec(AVCodecID codec_id) {
        switch (_io_type) {
        case ParamsType::Input:
            setCodec(utils::find_decoder(codec_id));
            return;
        case ParamsType::Output:
            setCodec(utils::find_encoder(codec_id));
            return;
        }
    }

    void Parameters::setCodec(std::string codec_short_name) {
        switch (_io_type) {
        case ParamsType::Input:
            setCodec(utils::find_decoder(codec_short_name));
            return;
        case ParamsType::Output:
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

    void Parameters::setStreamIndex(StreamId_t stream_index) {
        _stream_index = stream_index;
        setStreamUid(fpp::utils::gen_stream_uid(_context_uid, _stream_index));//TODO
    }

    void Parameters::setStreamUid(StreamId_t value) {
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

//    CodecType Parameters::codecType() const {
//        return _codec_type;
//    }

    int64_t Parameters::bitrate() const {
        return _bitrate;
    }

    int64_t Parameters::duration() const {
         return _duration;
    }

    StreamId_t Parameters::streamIndex() const {
        return _stream_index;
    }

    StreamId_t Parameters::streamUid() const {
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
        + "tb " + utils::rational_to_string(timeBase());
    }

    Code Parameters::completeFrom(const ParametersPointer other_params) {
        if (not_inited_codec_id(_codec_id)) {
//            try_to(utils::find_encoder_for(other_params, std::make_shared<Parameters>(this)));
            try_to(utils::find_encoder_for(other_params, ParametersPointer(this)));
        }

        if (not_inited_int(_bitrate)) { setBitrate(other_params->bitrate());     }
        if (not_inited_q(_time_base)) { setTimeBase(other_params->timeBase());   }
        return Code::OK;
    }

    void Parameters::parseStream(const AVStream* avstream) {
        setCodec(avstream->codecpar->codec_id);
        setBitrate(avstream->codecpar->bit_rate);
        setDuration(avstream->duration);
        setStreamIndex(avstream->index);
        setTimeBase(avstream->time_base);
    }

    bool Parameters::betterThen(const ParametersPointer& other) {
        return this->bitrate() > other->bitrate();
    }

} // namespace fpp
