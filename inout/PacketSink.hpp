#pragma once
#include "PacketProcessor.hpp"

namespace fpp {

    class PacketSink : public PacketProcessor {

    public:

        PacketSink();
        virtual ~PacketSink() override = default;

    private:

        Code                writePacket();

    };

    using PacketSinkPtr = std::unique_ptr<PacketSink>;

} // namespace fpp
