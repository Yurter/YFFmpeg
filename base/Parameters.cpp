#include "Parameters.hpp"
#include "utils.hpp"

namespace fpp {

    Parameters::Parameters(MediaType type) :
        MediaData(type),
        _codec(nullptr)
        , _codec_id(DEFAULT_CODEC_ID)
        , _codec_name(DEFAULT_STRING)
        , _bitrate(DEFAULT_INT)
        , _duration(DEFAULT_INT)
        , _stream_index(INVALID_INT)
        , _time_base(DEFAULT_RATIONAL)
        , _context_uid(INVALID_INT)
    {
        setName("Parameters");
    }

    void Parameters::setCodec(AVCodecID codec_id, CodecType codec_type) {
        switch (codec_type) {
        case CodecType::Decoder:
            setCodec(utils::find_decoder(codec_id));
            return;
        case CodecType::Encoder:
            setCodec(utils::find_encoder(codec_id));
            return;
        }
    }

    void Parameters::setCodec(std::string codec_short_name, CodecType codec_type) {
        switch (codec_type) {
        case CodecType::Decoder:
            setCodec(utils::find_decoder(codec_short_name));
            return;
        case CodecType::Encoder:
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
        _duration = duration;
    }

    void Parameters::setStreamIndex(int64_t stream_index) {
        _stream_index = stream_index;
    }

    void Parameters::setTimeBase(AVRational time_base) {
        _time_base = time_base;
    }

    void Parameters::setContextUid(int64_t context_uid) {
        _context_uid = context_uid;
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

    int64_t Parameters::bitrate() const {
        return _bitrate;
    }

    int64_t Parameters::duration() const {
        return _duration;
    }

    int64_t Parameters::streamIndex() const {
        return _stream_index;
    }

    AVRational Parameters::timeBase() const {
        return _time_base;
    }

    int64_t Parameters::contextUid() const {
        return _context_uid;
    }

    std::string Parameters::toString() const {
        std::string str = "TODO";
        return str;
    }

    void Parameters::softCopy(Parameters* other_parametrs) {
        if (not_inited_codec_id(_codec_id)) { _codec_id = other_parametrs->codecId();           }
        if (not_inited_string(_codec_name)) { _codec_name = other_parametrs->codecName();       }
        if (not_inited_int(_bitrate))       { _bitrate = other_parametrs->bitrate();            }
        if (not_inited_int(_duration))      { _duration = other_parametrs->duration();          }
        if (invalid_int(_stream_index))     { _stream_index = other_parametrs->streamIndex();   }
        if (not_inited_q(_time_base))       { _time_base = other_parametrs->timeBase();         }
    }

    Parameters& Parameters::operator=(const Parameters& rhs) {
        _codec_id =     rhs.codecId();
        _codec_name =   rhs.codecName();
        _bitrate =      rhs.bitrate();
        _duration =     rhs.duration();
        _stream_index = rhs.streamIndex();
        _time_base =    rhs.timeBase();
        return *this;
    }

} // namespace fpp
