#pragma once
#include "PacketProcessor.hpp"

namespace fpp {

    class PacketSource : public PacketProcessor {

    public:

        PacketSource();
        virtual ~PacketSource() override;

    private:

        Code                readPacket();

    };

    using PacketSourcePtr = std::unique_ptr<PacketSource>;

} // namespace fpp
