#include "PacketSource.hpp"

namespace fpp {

    PacketSource::PacketSource() {
        setName("PacketSource");
        try_throw(setPreFunction(std::bind(&PacketSource::readPacket, this)));
    }

    PacketSource::~PacketSource() {
        //TODO
    }

    Code PacketSource::readPacket() {
        Packet packet;
        try_to(readInputData(packet));
        try_to(sendOutputData(packet, this));
        return Code::OK;
    }

} // namespace fpp
