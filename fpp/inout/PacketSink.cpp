#include "PacketSink.hpp"

namespace fpp {

    PacketSink::PacketSink()
    {
        setName("PacketSink");
        setType(ProcessorType::Output);
        try_throw(setStartDataPred([](const Packet& packet) {
            return_if_not(packet.keyFrame(), Code::AGAIN);
            return Code::OK;
        }));
        try_throw(setPostFunction(std::bind(&PacketSink::writePacket, this)));
    }

    Code PacketSink::writePacket() {
        Packet packet;
        try_to(restoreOutputData(packet));
        try_to(writeOutputData(packet));
        increaseOutputDataCount();
        return Code::OK;
    }

} // namespace fpp
