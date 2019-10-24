#pragma once
#include "base/TemplateProcessor.hpp"
#include "base/Codec.hpp"
#include "base/Frame.hpp"
#include "base/Packet.hpp"

namespace fpp {

    class Encoder : public TemplateProcessor<Frame,Packet>, public Codec {

    public:

        Encoder(Stream* stream);
        virtual ~Encoder() override = default;

    private:

        virtual Code        initParams() override;
        virtual Code        processInputData(Frame input_data) override;

    };

    using EncoderList = std::list<Encoder*>;

} // namespace fpp
