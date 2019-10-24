#pragma once
#include "../base/PacketProcessor.hpp"

namespace fpp {

    class PacketSource : public TemplateProcessor<Packet,Packet> {

    public:

        PacketSource();
        virtual ~PacketSource() override;

    private:

//        Code                readPacket();

    };

} // namespace fpp
