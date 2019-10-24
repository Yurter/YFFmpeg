#include "PacketSink.hpp"

namespace fpp {

    PacketSink::PacketSink() {
        setName("PacketSink");
//        try_throw(setInOutFunction(std::bind(&PacketSink::writePacket, this)));
    }

} // namespace fpp
