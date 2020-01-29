#pragma once
#include <fpp/base/CodecContext.hpp>
#include <fpp/base/Frame.hpp>
#include <fpp/base/Packet.hpp>

namespace fpp {

    class DecoderContext : public CodecContext {

    public:

        DecoderContext(const SharedParameters parameters);
        virtual ~DecoderContext() override;

        FrameList           decode(const Packet& input_packet);
        virtual Code        flush(Object* data) override;

    };

} // namespace fpp
