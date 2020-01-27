#pragma once
#include <fpp/base/CodecContext.hpp>
#include <fpp/base/Frame.hpp>
#include <fpp/base/Packet.hpp>

namespace fpp {

    class DecoderContext : public CodecContext {

    public:

        DecoderContext(const IOParams params);
        virtual ~DecoderContext() override;

        Code                decode(Packet input_packet, Frame& decoded_frame);
        virtual Code        flush(Object* data)         override;

        virtual const ffmpeg::AVCodec*  codec()         override;
        virtual SharedParameters        parameters()    override;

    };

} // namespace fpp