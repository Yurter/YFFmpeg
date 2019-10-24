#include "PacketSource.hpp"

namespace fpp {

    PacketProducer::PacketProducer() {
        setName("PacketProducer");
        try_throw(setInOutFunction(std::bind(&PacketProducer::readPacket, this)));
    }

    Code PacketProducer::readPacket() {
        Packet packet;
        try_to(readInputData(packet));
        try_to(sendOutputData(packet, this));
        return Code::OK;
    }

} // namespace fpp
