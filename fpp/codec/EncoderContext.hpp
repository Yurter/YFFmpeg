#pragma once
#include <fpp/base/CodecContext.hpp>
#include <fpp/base/Frame.hpp>
#include <fpp/base/Packet.hpp>

namespace fpp {

    class EncoderContext : public CodecContext {

    public:

        EncoderContext(const SharedParameters parameters);
        virtual ~EncoderContext() override;

        Code                encode(Frame input_frame, Packet& encoded_packet);
        virtual Code        flush(Object* data) override;
        virtual Code        onOpen()            override;

    };

} // namespace fpp
