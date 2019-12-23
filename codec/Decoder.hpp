#pragma once
#include "inout/FrameProducer.hpp"
#include "DecoderContext.hpp"

namespace fpp {

    class Decoder : public FrameProducer {

    public:

        Decoder(const IOParams params);
        virtual ~Decoder() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual Code        processInputData(Packet input_data) override;
        virtual bool        equalTo(const ProcessorPointer other) const override;

        const DecoderContext* decoderContext() const;

    private:

        virtual Code        onStop() override;

    private:

        DecoderContext      _decoder_context;
        bool                _got_first_key_frame;

    };

    using DecoderList = std::list<Decoder*>;

} // namespace fpp
