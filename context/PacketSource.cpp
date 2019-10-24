#include "PacketSource.hpp"

namespace fpp {

    PacketProducer::PacketProducer() {
        setName("PacketProducer");
        try_throw(setInOutFunction(std::bind(&PacketProducer::readSourcePacket, this)));
    }

    Code PacketProducer::readSourcePacket() {
        return readInputData();
    }

} // namespace fpp
