#pragma once
#include "base/CodecContext.hpp"
#include "base/Frame.hpp"
#include "base/Packet.hpp"

namespace fpp {

    class EncoderContext : public CodecContext {

    public:

        EncoderContext(Stream* stream);
        virtual ~EncoderContext() override;

        Code                encode(Frame input_frame, Packet& output_packet);

    private:

        virtual Code        initParams() override;

    };

    using EncoderContextList = std::list<EncoderContext*>;

} // namespace fpp
