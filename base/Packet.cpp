#include "Packet.hpp"
#include "utils.hpp"

namespace fpp {

    Packet::Packet() :
        Data<AVPacket>(),
        _stream_uid(INVALID_INT)
    {
        setName("Packet");
    }

    Packet::~Packet() {
        // TODO ffmpeg ф-ии на отчистку
        //avpacket_unref(&m_raw);
    }

    Code Packet::init() {
        av_init_packet(&_data);
        setInited(true);
        return Code::OK;
    }

    void Packet::setPts(int64_t pts) {
        _data.pts = pts;
    }

    void Packet::setDts(int64_t dts) {
        _data.dts = dts;
    }

    void Packet::setDuration(int64_t duration) {
        _data.duration = duration;
    }

    void Packet::setPos(int64_t pos) {
        _data.pos = pos;
    }

    void Packet::setStreamIndex(int64_t stream_index) {
        _data.stream_index = static_cast<int>(stream_index);
    }

    void Packet::setStreamUid(int64_t stream_uid) {
        _stream_uid = stream_uid;
    }

    int64_t Packet::pts() const {
        return _data.pts;
    }

    int64_t Packet::dts() const {
        return _data.dts;
    }

    int64_t Packet::duration() const {
        return _data.duration;
    }

    int64_t Packet::pos() const {
        return _data.pos;
    }

    int64_t Packet::streamIndex() const {
        return static_cast<int64_t>(_data.stream_index);
    }

    int64_t Packet::streamUid() const {
        return _stream_uid;
    }

    bool Packet::empty() const {
        return _data.size == 0;
    }

    std::string Packet::toString() const {
        /* Video packet: 33123 bytes, dts 460, pts 460, duration 33 */
        std::string str = utils::media_type_to_string(type()) + " packet: "
                + std::to_string(_data.size) + " bytes, "
                + "dts " + utils::pts_to_string(_data.dts) + ", "
                + "pts " + utils::pts_to_string(_data.pts) + ", "
                + "duration " + std::to_string(_data.duration) + ", "
                + "stream index " + std::to_string(_data.stream_index) + ", "
                + "stream uid " + std::to_string(_stream_uid);
        return str;
    }

} // namespace fpp