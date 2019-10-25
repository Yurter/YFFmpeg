#pragma once
#include "base/TemplateProcessor.hpp"
#include "base/Codec.hpp"
#include "base/Frame.hpp"
#include "base/Packet.hpp"

namespace fpp {

    class Decoder : public Codec {

    public:

        Decoder(Stream* stream);
        virtual ~Decoder() override;

        Code                decode(Packet input_packet, Frame& output_frame);

    private:

        virtual Code        initParams() override;

    };

    using DecoderList = std::list<Decoder*>;

} // namespace fpp
