#pragma once
#include "base/CodecContext.hpp"
#include "base/Frame.hpp"
#include "base/Packet.hpp"

namespace fpp {

    class DecoderContext : public CodecContext {

    public:

        DecoderContext(const IOParams params);
        virtual ~DecoderContext() override;

        Code                decode(Packet input_packet, Frame& decoded_frame);
        virtual Code        flush(Object* data)         override;

    private:

        virtual Code        initParams()    override;

    };

} // namespace fpp
