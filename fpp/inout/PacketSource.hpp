#pragma once
#include <fpp/inout/PacketProcessor.hpp>

namespace fpp {

    class PacketSource : public PacketProcessor {

    public:

        PacketSource();
        virtual ~PacketSource() override;

    private:

        Code                readPacket();

    };

} // namespace fpp
