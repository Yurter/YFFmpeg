#include "Packet.hpp"
#include "core/utils.hpp"

namespace fpp {

    Packet::Packet() :
        Data<AVPacket>(),
        _stream_uid(INVALID_INT)
    {
        setName("Packet");
    }

    Packet::Packet(const Packet& other) {
        setName("Packet");
        setType(other.type());
        setPts(other.pts());
        setDts(other.dts());
        setDuration(other.duration());
        setPos(other.pos());
        setStreamIndex(other.streamIndex());
        setStreamUid(other.streamUid());
        if (av_packet_ref(&_data, &other._data) != 0) {
            log_error("av_packet_ref failed! " << other);
        }
        setInited(true);
    }

    Packet::Packet(const AVPacket& avpacket) {
        setPts(avpacket.pts);
        setDts(avpacket.dts);
        setDuration(avpacket.duration);
        setPos(avpacket.pos);
        setStreamIndex(avpacket.stream_index);
        if (av_packet_ref(&_data, &avpacket) != 0) {
            log_error("av_packet_ref failed! copyFrom().");
        }
        setInited(true);
    }

    Packet::~Packet() {
        av_packet_unref(&_data);
    }

    Packet& Packet::operator=(const Packet& other) {
        setName("Packet");
        setType(other.type());
        setPts(other.pts());
        setDts(other.dts());
        setDuration(other.duration());
        setPos(other.pos());
        setStreamIndex(other.streamIndex());
        setStreamUid(other.streamUid());
        if (av_packet_ref(&_data, &other._data) != 0) {
            log_error("av_packet_ref failed! " << other);
        }
        setInited(true);
        return *this;
    }

    Packet& Packet::operator=(const Packet&& other) {
        setName("Packet");
        setType(other.type());
        setPts(other.pts());
        setDts(other.dts());
        setDuration(other.duration());
        setPos(other.pos());
        setStreamIndex(other.streamIndex());
        setStreamUid(other.streamUid());
        if (av_packet_ref(&_data, &other._data) != 0) {
            log_error("av_packet_ref failed! " << other);
        }
        setInited(true);
        return *this;
    }

//    void Packet::copyFrom(const AVPacket& avpacket) {
//        setPts(avpacket.pts);
//        setDts(avpacket.dts);
//        setDuration(avpacket.duration);
//        setPos(avpacket.pos);
//        setStreamIndex(avpacket.stream_index);
//        if (av_packet_ref(&_data, &avpacket) != 0) {
//            log_error("av_packet_ref failed! copyFrom().");
//        }
//        setInited(true);
//    }

//    void Packet::cloneFrom(const AVPacket& avpacket) {
//        setPts(avpacket.pts);
//        setDts(avpacket.dts);
//        setDuration(avpacket.duration);
//        setPos(avpacket.pos);
//        setStreamIndex(avpacket.stream_index);
//        if (av_packet_ref(&_data, &avpacket) != 0) {
//            log_error("av_packet_ref failed! copyFrom().");
//        }
//    AVPacket newPacket(oldPacket);
//    newPacket->data = reinterpret_cast<uint8_t*>(new uint64_t[(oldPacket->size + FF_INPUT_BUFFER_PADDING_SIZE)/sizeof(uint64_t) + 1]);
//    memcpy(newPacket->data, oldPacket->data, oldPacket->size);
//        setInited(true);
//    }

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

//    bool Packet::empty() const {
//        return size() == 0;
//    }

    uint64_t Packet::size() const {
        return uint64_t(_data.size);
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
