#pragma once
#include "base/CodecContext.hpp"
#include "base/Frame.hpp"
#include "base/Packet.hpp"

namespace fpp {

    class DecoderContext : public CodecContext {

    public:

        DecoderContext(const IOParams params);
        virtual ~DecoderContext() override;

        Code                decode(Packet input_packet, Frame& output_frame); //TODO rename 'output_frame' to 'decoded_frame' 10.01
        virtual Code        flush(Object* data)         override;

    private:

        virtual Code        initParams()    override;

    };

    using DecoderContextList = std::list<DecoderContext*>;

} // namespace fpp
