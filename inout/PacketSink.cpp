#include "PacketSink.hpp"

namespace fpp {

    PacketSink::PacketSink()
    {
        setName("PacketSink");
        try_throw(setPostFunction(std::bind(&PacketSink::writePacket, this)));
    }

    PacketSink::~PacketSink() {
        // TODO
    }

    Code PacketSink::writePacket() {
        Packet packet;
        try_to(restoreOutputData(packet));
        try_to(writeOutputData(packet));
        return Code::OK;
    }

} // namespace fpp
