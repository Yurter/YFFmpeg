#include "Stream.hpp"
#include "core/utils.hpp"

namespace fpp {

    Stream::Stream(Parameters* param) :
        Stream(nullptr, param)
    {
        EMPTY_CONSTRUCTOR
    }

    Stream::Stream(AVStream* stream, Parameters* param) :
        Data<AVStream*>(stream, param->type()),
        params(param),
        _used(false),
        _context(nullptr),
        _prev_dts(DEFAULT_INT),
        _prev_pts(DEFAULT_INT),
        _packet_index(DEFAULT_INT),
        _packet_dts_delta(INVALID_INT),
        _packet_pts_delta(INVALID_INT),
        _packet_duration(INVALID_INT)
    {
        setName("Stream");
    }

    Stream::~Stream() {
        delete params;
    }

    Code Stream::init() {
        if (inited_ptr(raw())) {
            /* Иницаиализация полей параметров кодека дефолтными значениями */
            try_to(utils::init_codecpar(codecParameters(), params->codec()));
            /* Инициализация полей параметров кодека значениями из параметров потока */
            utils::parameters_to_avcodecpar(params, codecParameters());
        }
        setInited(true);
        return Code::OK;
    }

    std::string Stream::toString() const {
        return_if_not(inited(), "not inited!");

        std::string str = "[" + std::to_string(params->contextUid())
                + ":" + std::to_string(params->streamIndex()) + "] "
                + utils::media_type_to_string(type()) + " stream: ";

        str += used() ? params->toString() : "not used";
        return str;
    }

    Code Stream::stampPacket(Packet& packet) {
        return_if(packet.type() != type(), Code::INVALID_INPUT);

        packet.setDts(_prev_dts);
        packet.setPts(_prev_pts);
        packet.setDuration(_packet_duration);
        packet.setPos(-1);

        _prev_dts += _packet_dts_delta;
        _prev_pts += _packet_pts_delta;
        _packet_index++;

        params->increaseDuration(_packet_duration);

        return Code::OK;
    }

    void Stream::setContext(Object* context) {
        _context = context;
    }

    void Stream::setUid(int64_t uid){
        if (invalid_int(params->streamUid())) {
            params->setStreamUid(uid);
        } else {
            log_warning("Cannot set uid again, ignored");
        }
    }

    void Stream::setUsed(bool used) {
        _used = used;
    }

    Object* Stream::context() const {
        return _context;
    }

    int64_t Stream::index() const {
        return _data->index;
    }

    bool Stream::used() const {
        return _used;
    }

    int64_t Stream::packetIndex() const {
        return _packet_index;
    }

    AVCodecParameters* Stream::codecParameters() {
        return_if(not_inited_ptr(_data), nullptr);
        return _data->codecpar;
    }

    bool Stream::operator>(const Stream& other) const {
        return params->bitrate() > other.params->bitrate();
    }

    void Stream::parseParameters() //TODO перенести код из FormatContext::parseFormatContext()
    {
        return_error_if(not_inited_ptr(_data)
                        , "Cannot parse virtual stream."
                        , void());
        _data->time_base = params->timeBase();
    }

} // namespace fpp
