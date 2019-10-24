#pragma once
#include "PacketProcessor.hpp"

namespace fpp {

    class PacketSink : public PacketProcessor {

    public:

        PacketSink();
        virtual ~PacketSink() override;

//        Code                writePacket();

    };

} // namespace fpp
