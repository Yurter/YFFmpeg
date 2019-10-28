#pragma once
#include "base/CodecContext.hpp"
#include "base/Frame.hpp"
#include "base/Packet.hpp"

namespace fpp {

    class DecoderContext : public CodecContext {

    public:

        DecoderContext(Stream* stream);
        virtual ~DecoderContext() override;

        Code                decode(Packet input_packet, Frame& output_frame);

    private:

        virtual Code        initParams() override;

    };

    using DecoderContextList = std::list<DecoderContext*>;

} // namespace fpp
