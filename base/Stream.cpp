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
        case StampType::ConstantFramerate: /* Константный фреймрейт */
            break;
        case StampType::VariableFramerate: /* Переменный фреймрейт  */
            break;
        case StampType::Copy: /* Не менять параметры пакета */ //TODO
            params->increaseDuration(_packet_duration);
            _packet_index++;
            return Code::OK;
        case StampType::Append: { /* Используется при склейки файлов */
            auto new_pts = packet.pts() + _pts_offset;
            if (new_pts <= _prev_pts) {
                _pts_offset = params->duration();
                new_pts = packet.pts() + _pts_offset;
            }
            _packet_duration = new_pts - _prev_pts;
            break;
        }
        case StampType::Convert: //TODO
            packet.setPts(av_rescale_q(packet.pts(), DEFAULT_TIME_BASE, params->timeBase()));
            break;
        case StampType::Realtime: { /* Временные штампы реального времени */
            if (_packet_index == 0) { //TODO
                _chronometer.reset_timepoint();
                _packet_duration = 40;
            } else {
                _packet_duration = _chronometer.elapsed_milliseconds();
            }
            _packet_dts_delta = _packet_duration;
            _packet_dts_delta = _packet_duration;
            _chronometer.reset_timepoint();
            break;
        }
        }

        packet.setDts(_prev_dts);
        packet.setPts(_prev_pts);
        packet.setDuration(_packet_duration);
        packet.setPos(-1);
        params->increaseDuration(_packet_duration);

        _prev_dts += _packet_dts_delta;
        _prev_pts += _packet_pts_delta;
        _packet_index++;

        return Code::OK;
    }

//    Code Stream::stampPacket(Packet& packet, StreamCrutch mode) { //TODO штампы в реалтайме
//        return_if(packet.type() != type(), Code::INVALID_INPUT);

//        auto new_pts = packet.pts() + _local_pts;
//        if (new_pts < _prev_pts) {
//            _local_pts = parameters->duration();
//            new_pts = packet.pts() + _local_pts;
//        }
//        _packet_duration = (new_pts - _prev_pts) == 0 ? 10 : (new_pts - _prev_pts);
//        parameters->increaseDuration(_packet_duration);

//        packet.setDts(new_pts);
//        packet.setPts(new_pts);
//        packet.setDuration(_packet_duration);
//        packet.setPos(-1);

//        _prev_dts = packet.pts();
//        _prev_pts = packet.pts();
//        _packet_index++;

//        return Code::OK;
//    }
//    Code Stream::stampPacket(Packet& packet, StreamCrutch mode) { //TODO штампы в реалтайме
//        return_if(packet.type() != type(), Code::INVALID_INPUT);

//        if (_packet_index == 0) {
//            _chronometer.reset_timepoint();
//        }

//        if ((_packet_index != 0) && (mode == StreamCrutch::RealTyme)) {
//            auto time_delta = _chronometer.elapsed_milliseconds();
//            if (time_delta < 10) {
//                time_delta = 40; /* cructh */
//            }
//            _packet_duration = time_delta;
//            _packet_dts_delta = _packet_pts_delta = _packet_duration;
//            _chronometer.reset_timepoint();
//            _prev_dts += _packet_dts_delta;
//            _prev_pts += _packet_pts_delta;
//        } else if (mode == StreamCrutch::Append) {
//            auto new_pts = packet.pts() + _local_pts;
//            if (new_pts < _prev_pts) {
//                _local_pts = params->duration();
//                new_pts = packet.pts() + _local_pts;
//            }
//            _packet_duration = (new_pts - _prev_pts) == 0 ? 10 : (new_pts - _prev_pts);
//            params->increaseDuration(_packet_duration);

//            packet.setDts(new_pts);
//            packet.setPts(new_pts);
//            packet.setDuration(_packet_duration);
//            packet.setPos(-1);

//            _prev_dts = packet.pts();
//            _prev_pts = packet.pts();
//            _packet_index++;

//            return Code::OK;
//        } else if (mode == StreamCrutch::Tmls) {
//            auto time_delta = _chronometer.elapsed_milliseconds();
//            if (time_delta < 10) {
//                time_delta = 40; /* cructh */
//            }
//            time_delta /= 60;
//            _packet_duration = time_delta;
//            _packet_dts_delta = _packet_pts_delta = _packet_duration;
//            _chronometer.reset_timepoint();
//            _prev_dts += _packet_dts_delta;
//            _prev_pts += _packet_pts_delta;
//        } else {
//            _prev_dts += _packet_dts_delta;
//            _prev_pts += _packet_pts_delta;
//        }

//        packet.setDts(_prev_dts);
//        packet.setPts(_prev_pts);
//        packet.setDuration(_packet_duration);
//        packet.setPos(-1);

//        _packet_index++;

//        params->increaseDuration(_packet_duration);

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
