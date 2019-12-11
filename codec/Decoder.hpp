#pragma once
#include "inout/FrameProducer.hpp"
#include "DecoderContext.hpp"

namespace fpp {

    class Decoder : public FrameProducer {

    public:

        Decoder(const IOParams params);

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual Code        processInputData(Packet input_data) override;
        virtual bool        equalTo(const ProcessorPointer other) const override;

        const DecoderContext* decoderContext() const;

    private:

        DecoderContext      _decoder_context;

    };

    using DecoderList = std::list<Decoder*>;

} // namespace fpp
