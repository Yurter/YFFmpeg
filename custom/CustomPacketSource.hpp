#pragma once
#include "inout/PacketSource.hpp"

namespace fpp {

    class CustomPacketSource : public PacketSource {

    public:

        CustomPacketSource(const std::string mrl, IOType preset = IOType::Auto);
        virtual ~CustomPacketSource() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

//        VideoStream*        videoStream();

    private:

        virtual Code        readInputData(Packet& input_data) override;
        virtual Code        processInputData(Packet input_data) override;
        virtual Code        onStop() override;

        Code                sendEofPacket();

    };

//    using MediaSourceList = std::list<MediaSource*>;

} // namespace fpp
