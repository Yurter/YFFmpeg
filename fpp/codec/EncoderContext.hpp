#pragma once
#include <fpp/base/CodecContext.hpp>
#include <fpp/base/Frame.hpp>
#include <fpp/base/Packet.hpp>

namespace fpp {

    class EncoderContext : public CodecContext {

    public:

        EncoderContext(const SharedParameters parameters, AVRational source_time_base);
        virtual ~EncoderContext() override;

        PacketList          encode(Frame input_frame);
        virtual Code        flush(Object* data) override;
        virtual Code        onOpen()            override;

    private:

        AVRational          _source_time_base;

    };

} // namespace fpp
