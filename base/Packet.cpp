#include "Packet.hpp"
#include "core/utils.hpp"

namespace fpp {

    Packet::Packet()
        : Data<AVPacket>()
        , _time_base { DEFAULT_RATIONAL }
    {
        setName("Packet");
    }

    Packet::Packet(const Packet& other) {
        setName("Packet");
        copy(other);
        setInited(true);
    }

    Packet::Packet(const Packet&& other) {
        setName("Packet");
        copy(other);
        setInited(true);
    }

    Packet::Packet(const AVPacket& avpacket) { //TODO refactoring 14.01
        setPts(avpacket.pts);
        setDts(avpacket.dts);
        setDuration(avpacket.duration);
        setPos(avpacket.pos);
        setStreamIndex(avpacket.stream_index);
        if (av_packet_ref(&_data, &avpacket) != 0) {
            log_error("av_packet_ref failed!");
            return;
        }
        setInited(true);
    }

    Packet::~Packet() {
        av_packet_unref(&_data);
    }

    Packet& Packet::operator=(const Packet& other) {
        copy(other);
        setInited(true);
        return *this;
    }

    Packet& Packet::operator=(const Packet&& other) {
        copy(other);
        setInited(true);
        return *this;
    }

    Code Packet::init() { //TODO убрать за ненадобностью? как
//        av_init_packet(&_data);
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

    void Packet::setTimeBase(AVRational time_base) {
        _time_base = time_base;
    }

    void Packet::setPos(int64_t pos) {
        _data.pos = pos;
    }

    void Packet::setStreamIndex(int64_t stream_index) {
        _data.stream_index = int(stream_index);
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

    AVRational Packet::timeBase() const {
        return _time_base;
    }

    int64_t Packet::pos() const {
        return _data.pos;
    }

    int64_t Packet::streamIndex() const {
        return int64_t(_data.stream_index);
    }

    bool Packet::keyFrame() const {
        return _data.flags & AV_PKT_FLAG_KEY;
    }

    uint64_t Packet::size() const {
        return uint64_t(_data.size);
    }

    std::string Packet::toString() const {
        /* Video packet: [_], 1516 bytes, dts 1709, pts 1709, duration 29, time_base 1/90000, stream index 0    */
        /* Audio packet: [I], 1045 bytes, dts 392, pts 392, duration 26, time_base 1/44100, stream index 1      */
        return utils::media_type_to_string(type()) + " packet: "
                + (keyFrame() ? "[I]" : "[_]") + ", "
                + std::to_string(_data.size) + " bytes, "
                + "dts " + utils::pts_to_string(_data.dts) + ", "
                + "pts " + utils::pts_to_string(_data.pts) + ", "
                + "duration " + std::to_string(_data.duration) + ", "
                + "time_base " + utils::rational_to_string(_time_base) + ", "
                + "stream index " + std::to_string(_data.stream_index);
    }

    void Packet::copy(const Packet& other) {
        setType(other.type());
        setPts(other.pts());
        setDts(other.dts());
        setDuration(other.duration());
        setTimeBase(other.timeBase());
        setPos(other.pos());
        setStreamIndex(other.streamIndex());
        if (av_packet_ref(&_data, &other._data) != 0) {
            log_error("av_packet_ref failed, " << other);
        }
    }

} // namespace fpp
