#include "Stream.hpp"
#include "core/utils.hpp"

namespace fpp {

    Stream::Stream(Parameters* param) :
        Stream(nullptr, param)
    {
        EMPTY_CONSTRUCTOR
    }

    Stream::Stream(AVStream* stream, Parameters* param) :
        Data<AVStream*>(stream, param->type())
        , params(param)
        , _used(false)
        , _stamp_type(StampType::ConstantFramerate)
        , _prev_dts(DEFAULT_INT)
        , _prev_pts(DEFAULT_INT)
        , _packet_index(DEFAULT_INT)
        , _packet_dts_delta(INVALID_INT)
        , _packet_pts_delta(INVALID_INT)
        , _packet_duration(INVALID_INT)
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
        switch (_stamp_type) {
        case StampType::Realtime: {
            { //TODO костыль сброса таймера на получении первого пакета
                if (_packet_index == 0) {
                    _chronometer.reset_timepoint();
                    _packet_duration = 40;
                } else {
                    _packet_duration = _chronometer.elapsed_milliseconds();
                }
            }

            params->increaseDuration(_packet_duration);

            _packet_dts_delta = _packet_duration;
            _packet_pts_delta = _packet_duration;
            _chronometer.reset_timepoint();

            auto new_dts = _prev_dts + _packet_dts_delta;
            auto new_pts = _prev_pts + _packet_pts_delta;

            _prev_dts = new_dts;
            _prev_pts = new_pts;

            new_dts = av_rescale_q(new_dts, DEFAULT_TIME_BASE, params->timeBase());
            new_pts = av_rescale_q(new_pts, DEFAULT_TIME_BASE, params->timeBase());
            _packet_duration = av_rescale_q(_packet_duration, DEFAULT_TIME_BASE, params->timeBase());

            packet.setDts(new_dts);
            packet.setPts(new_pts);
            packet.setDuration(_packet_duration);
            packet.setPos(-1);

            break;
        }
        default:
            return Code::NOT_IMPLEMENTED;
        }

        _packet_index++;
        return Code::OK;
    }
//    Code Stream::stampPacket(Packet& packet) {
//        switch (_stamp_type) {
//        case StampType::ConstantFramerate: /* Константный фреймрейт */
//            packet.setDts(_prev_dts + _packet_duration);
//            packet.setPts(_prev_pts + _packet_duration);
//            break;
//        case StampType::VariableFramerate: { /* Переменный фреймрейт  */
//            break;
//        }
//        case StampType::Offset: {
//            if (packetIndex() == 0) {
//                _pts_offset = -packet.pts();
//            }
//            auto new_pts = packet.pts() + _pts_offset;
//            auto new_dts = packet.dts() + _pts_offset;
////            if (new_pts <= _prev_pts) {
////                _pts_offset = params->duration();
////                new_pts = packet.pts() + _pts_offset;
////            }
//            _packet_duration = new_pts - _prev_pts;
//            _packet_dts_delta = _packet_duration;
//            _packet_pts_delta = _packet_duration;
//            packet.setDts(new_dts);
//            packet.setPts(new_pts);
//            break;
//        }
//        case StampType::Copy: /* Не менять параметры пакета */ //TODO
//            params->increaseDuration(_packet_duration);
//            _packet_index++;
//            return Code::OK;
//        case StampType::Append: { /* Используется при склейки файлов */
////            log_debug("PACKET PTS: " << packet.pts());
//            auto new_pts = packet.pts() + _pts_offset;
//            auto new_dts = packet.dts() + _pts_offset;
//            if (new_pts <= _prev_pts) {
////                log_error("OP");
//                _pts_offset = params->duration();
//                new_pts = packet.pts() + _pts_offset;
//                new_dts = packet.dts() + _pts_offset;
//            }
//            _packet_duration = new_pts - _prev_pts;
//            if (_packet_duration == 0) { _packet_duration = 40; }
//            _packet_dts_delta = _packet_duration;
//            _packet_pts_delta = _packet_duration;
//            packet.setDts(new_dts);
//            packet.setPts(new_pts);
//            break;
//        }
//        case StampType::Rescale: {
////            _prev_pts = av_rescale_q(packet.pts(), DEFAULT_TIME_BASE, params->timeBase());
////            _prev_dts = av_rescale_q(packet.dts(), DEFAULT_TIME_BASE, params->timeBase());
//            _packet_duration = packet.pts() - _prev_pts;
//            _packet_dts_delta = _packet_duration;
//            _packet_pts_delta = _packet_duration;
////            log_debug(_packet_duration);
//            break;
//        }
//        case StampType::Realtime: { /* Временные штампы реального времени */
//            if (_packet_index == 0) { //TODO
//                _chronometer.reset_timepoint();
//                _packet_duration = 40;
//            } else {
//                _packet_duration = _chronometer.elapsed_milliseconds();
//            }

//            if (_packet_duration < 10) { _packet_duration = 40; }

//            _packet_dts_delta = _packet_duration;
//            _packet_pts_delta = _packet_duration;
//            _chronometer.reset_timepoint();

//            auto new_dts = _prev_dts + _packet_dts_delta;
//            auto new_pts = _prev_pts + _packet_pts_delta;

//            if (_packet_index == 0) {
//                new_dts = new_pts = 0;
//            }

//            packet.setDts(new_dts);
//            packet.setPts(new_pts);
//            break;
//        }
//        }

////        packet.setDts(_prev_dts);
////        packet.setPts(_prev_pts);
//        packet.setDuration(_packet_duration);
//        packet.setPos(-1);
//        params->increaseDuration(_packet_duration);
//        if (_stamp_type == StampType::Append) {
////            log_debug("PACKET PTS: " << packet.pts() << " " << packet.dts());
//        }

////        _prev_dts += _packet_dts_delta;
////        _prev_pts += _packet_pts_delta;
//        _prev_dts = packet.dts();
//        _prev_pts = packet.pts();
//        _packet_index++;

//        return Code::OK;
//    }

    void Stream::setUsed(bool value) {
        _used = value;
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
