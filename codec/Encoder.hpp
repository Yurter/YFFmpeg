#pragma once
#include "base/TemplateProcessor.hpp"
#include "base/Codec.hpp"
#include "base/Frame.hpp"
#include "base/Packet.hpp"

namespace fpp {

    class Encoder : /*public TemplateProcessor<Frame,Packet>,*/ public Codec {

    public:

        Encoder(Stream* stream);
        virtual ~Encoder() override;

        Code                encode(Frame input_frame, Packet& output_packet);

    private:

        virtual Code        initParams() override;

    };

    using EncoderList = std::list<Encoder*>;

} // namespace fpp
