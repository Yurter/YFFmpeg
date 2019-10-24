#pragma once
#include "base/TemplateProcessor.hpp"
#include "base/Codec.hpp"
#include "base/Frame.hpp"
#include "base/Packet.hpp"

namespace fpp {

    class Decoder : public TemplateProcessor<Packet,Frame>, public Codec {

    public:

        Decoder(Stream* stream);
        virtual ~Decoder() override = default;

    private:

        virtual Code        initParams() override;
        virtual Code        processInputData(Packet input_data) override;

    };

    using DecoderList = std::list<Decoder*>;

} // namespace fpp
