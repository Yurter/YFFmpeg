#include "Stream.hpp"
#include "core/utils.hpp"

namespace fpp {

    Stream::Stream()
        : _used(false)
        , _stamp_type(StampType::Rescale)
        , _prev_dts(DEFAULT_INT)
        , _prev_pts(DEFAULT_INT)
        , _packet_index(DEFAULT_INT)
        , _packet_dts_delta(INVALID_INT)
        , _packet_pts_delta(INVALID_INT)
        , _packet_duration(INVALID_INT)
        , _pts_offset(DEFAULT_INT)
        , _dts_offset(DEFAULT_INT)
        , _start_time_point(FROM_START)
        , _end_time_point(TO_END) {
        setType(params->type());
        setName(utils::media_type_to_string(type()) + " stream");
    }

    Stream::Stream(const AVStream* avstream)
        : Stream() {
        params->parseStream(avstream);
    }

    Stream::Stream(Parameters* param)
        : Data<AVStream*>(nullptr, param->type())
        , params(param)
    {
    }

    Stream::~Stream() {
        delete params; //TODO в шаред поинтер 14.01
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

        return "[" + std::to_string(params->contextUid())
                + ":" + std::to_string(params->streamIndex()) + "] "
                + utils::media_type_to_string(type()) + " stream: "
                + (used() ? params->toString() : "not used");
    }

    void Stream::determineStampType(const Packet& packet) { //TODO: перенести в сорс, т.к. только в нем изменяется дефолтный тип штампа
        if (packet.pts() != 0) { /* Требуется перештамповывать пакеты */
            if (_start_time_point == FROM_START) { /* Чтение из источника, передающего пакеты не с начала */
                setStampType(StampType::Realtime);
            } else {
                setStampType(StampType::Offset); /* Происходит чтение не с начала файла */
            }
        } else {
            setStampType(StampType::Copy); // (нет смысла рескейлить, т.к. этот метод вызывается отлько на входе)
//            /* Валидные штампы, проверка на необходимость рескейла */
//            if (utils::equal_rational(packet.timeBase(), params->timeBase())) {
//                setStampType(StampType::Copy);
//            } else {
//                setStampType(StampType::Rescale); //мысля не работает как надо, потому что этот метод вызывается только на входе
//            }
        }
    }

    Code Stream::stampPacket(Packet& packet) { // TODO refactoring 14.01
        switch (_stamp_type) {
        case StampType::Copy:
            _packet_duration = packet.pts() - _prev_pts;
            break;
        case StampType::Realtime: {
            if (_packet_index == 0) { //TODO костыль сброса таймера на получении первого пакета, перенести в открытие?
                _chronometer.reset_timepoint();
            }

            const AVRational chronometer_timebase = DEFAULT_TIME_BASE;
            _packet_duration = av_rescale_q(_chronometer.elapsed_milliseconds(), chronometer_timebase, params->timeBase());

            _chronometer.reset_timepoint();

            if (_packet_duration < 16) { //TODO костыль: ффмпег отдает первый кадров 10 мгновенно
                const int64_t duration_ms = int64_t(1000 / av_q2d(static_cast<VideoParameters*>(params)->frameRate()));
                _packet_duration = av_rescale_q(duration_ms, DEFAULT_TIME_BASE, params->timeBase());
            }

            if (_packet_index == 0) { //TODO костыль
                packet.setDts(0);
                packet.setPts(0);
                break;
            }

            _packet_dts_delta = _packet_duration;
            _packet_pts_delta = _packet_duration;

            packet.setDts(_prev_dts + _packet_dts_delta);
            packet.setPts(_prev_pts + _packet_pts_delta);
            break;
        }
        case StampType::Rescale: {
            auto debug_value_00 = packet.pts();
//            if (packet.isAudio()) log_warning("res from " << packet.timeBase() << " to " << params->timeBase());
            /* Рескеил в таймбейс потока без изменений */
            packet.setDts(av_rescale_q(packet.dts(), packet.timeBase(), params->timeBase()));
            packet.setPts(av_rescale_q(packet.pts(), packet.timeBase(), params->timeBase()));
//            packet.setDts(av_rescale_q(packet.dts(), packet_time_base, params->timeBase()));
//            packet.setPts(av_rescale_q(packet.pts(), packet_time_base, params->timeBase()));
            auto debug_value_01 = packet.pts();

            if (packetIndex() == 0) {
                _pts_offset = -packet.pts();
                _dts_offset = -packet.dts();
//                log_error("OFFSET: " << _pts_offset);
            }

            auto debug_value = packet.pts();

            /* Пересчет с учетом смещения */
            auto new_pts = packet.pts() + _pts_offset;
            auto new_dts = packet.dts() + _dts_offset;

            /* Проверка на начало новой последовательности пакетов */
            if (new_pts < _prev_pts) {
                auto offset = params->duration();
                _pts_offset = offset;
                _dts_offset = offset;
                new_pts = packet.pts() + _pts_offset;
                new_dts = packet.dts() + _dts_offset;
                log_error("new_pts < _prev_pts: " << new_pts << " " << offset);
                log_error("source pts: " << debug_value << ", " << debug_value_00 << " -> " << debug_value_01);
            }

            /* Расчет длительности пакета */
            _packet_duration = new_pts - _prev_pts;

            _packet_dts_delta = _packet_duration;
            _packet_pts_delta = _packet_duration;


            /* Установка новых значений */
            packet.setDts(new_dts);
            packet.setPts(new_pts);
            break;
        }
        case StampType::Offset: {
            if (packetIndex() == 0) {
                _pts_offset = -packet.pts();
                _dts_offset = -packet.dts();
            }
            auto new_pts = packet.pts() + _pts_offset;
            auto new_dts = packet.dts() + _dts_offset;
            _packet_duration = new_pts - _prev_pts;
            _packet_dts_delta = _packet_duration;
            _packet_pts_delta = _packet_duration;
            packet.setDts(new_dts);
            packet.setPts(new_pts);
            break;
        }
        }

        packet.setPos(-1);
        packet.setDuration(_packet_duration);
        packet.setTimeBase(params->timeBase());
        params->increaseDuration(packet.duration());
        _prev_dts = packet.dts();
        _prev_pts = packet.pts();
        _packet_index++;
        return Code::OK;
    }

    bool Stream::timeIsOver() const {
        const auto planned_duration = _end_time_point - _start_time_point;
        const auto actual_duration = av_rescale_q(params->duration(), params->timeBase(), DEFAULT_TIME_BASE);
        if (actual_duration >= planned_duration) {
            log_debug("Time is over: " << utils::time_to_string(actual_duration, DEFAULT_TIME_BASE));
            return true;
        }
        return false;
    }

    void Stream::setUsed(bool value) {
        _used = value;
    }

    void Stream::setStampType(StampType value) {
        if (value == StampType::Rescale) {
            setName(name());
        }
        _stamp_type = value;
    }

    void Stream::setStartTimePoint(int64_t value) {
        return_if(_start_time_point == value, void());
        if ((value != FROM_START) && (value < 0)) {
            log_warning("Cannot set start time point less then zero: " << value << ", ignored");
            return;
        }
        if ((_end_time_point != TO_END) && (value > _end_time_point)) {
            log_warning("Cannot set start time point more then end time point "
                        << _end_time_point <<  ": " << value << ", ignored");
            return;
        }
        _start_time_point = value;
    }

    void Stream::setEndTimePoint(int64_t value) {
        return_if(_end_time_point == value, void());
        if ((value != TO_END) && (value < 0)) {
            log_warning("Cannot set end time point less then zero: " << value << ", ignored");
            return;
        }
        if ((_start_time_point != FROM_START) && (value < _start_time_point)) {
            log_warning("Cannot set end time point less then start time point "
                        << _start_time_point <<  ": " << value << ", ignored");
            return;
        }
        _end_time_point = value;
    }

    int64_t Stream::index() const {
        return _data->index;
    }

    bool Stream::used() const {
        return _used;
    }

    StampType Stream::stampType() const {
        return _stamp_type;
    }

    int64_t Stream::startTimePoint() const {
        return _start_time_point;
    }

    int64_t Stream::endTimePoint() const {
        return _end_time_point;
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

} // namespace fpp
