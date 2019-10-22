#include "Stream.hpp"
#include "utils.hpp"

namespace fpp {

    Stream::Stream(Parameters* param) :
        Stream(nullptr, param)
    {
        EMPTY_CONSTRUCTOR
    }

    Stream::Stream(AVStream* stream, Parameters* param) :
        Data<AVStream*>(stream, param->type()),
        parameters(param),
        _context(nullptr),
        _uid(INVALID_INT),
        _duration(DEFAULT_INT),
        _used(false),
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
        delete parameters;
    }

    Code Stream::init() {
        /* Иницаиализация полей параметров кодека дефолтными значениями */
        try_to(utils::init_codecpar(codecParameters(), parameters->codec()));
        /* Инициализация полей параметров кодека значениями из параметров потока */
        utils::parameters_to_avcodecpar(parameters, codecParameters());
        return Code::OK;
    }

    std::string Stream::toString() const { //TODO дампить параметры
        return_if_not(inited(), "not inited!");
        std::string str = "[" + std::to_string(parameters->contextUid())
                + ":" + std::to_string(parameters->streamIndex()) + "] "
                + utils::media_type_to_string(type()) + " stream: "
                + parameters->codecName() + ", ";

        if (used()) {
            str += "dts_delta " + std::to_string(_packet_dts_delta) + ", "
                    + "pts_delta " + std::to_string(_packet_pts_delta) + ", "
    //              + "time_base " + utils::rational_to_string(timeBase()) + ", "
                    + "time_base " + utils::rational_to_string(parameters->timeBase()) + ", "
//                    + "FPS " + std::to_string(static_cast<VideoParameters*>(parameters)->frameRate()) + ", "
                    + "duration " + std::to_string(_duration);
        } else {
            str += "not used";
        }

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
        _duration += _packet_duration;
        _packet_index++;

        return Code::OK;
    }

    void Stream::setContext(Object* context) {
        _context = context;
    }

    void Stream::setUid(int64_t uid){
        if (invalid_int(_uid)) { _uid = uid; }
    }

    void Stream::setUsed(bool used) {
        _used = used;
    }

    Object* Stream::context() const {
        return _context;
    }

    int64_t Stream::uid() const {
        return _uid;
    }

    int64_t Stream::index() const {
        return _data->index;
    }

    int64_t Stream::duration() const {
        return _duration;
    }

    bool Stream::used() const {
        return _used;
    }

    AVCodecParameters* Stream::codecParameters() {
        return _data->codecpar;
    }

    void Stream::increaseDuration(int64_t value){
        _duration += value;
    }

    bool Stream::operator>(const Stream& other) const {
        return parameters->bitrate() > other.parameters->bitrate();
    }

    void Stream::parseParametres() //TODO ? што это?
    {
        _data->time_base = parameters->timeBase();
    }

} // namespace fpp
