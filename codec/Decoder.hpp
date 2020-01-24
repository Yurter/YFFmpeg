#pragma once
#include <inout/FrameProducer.hpp>
#include <codec/DecoderContext.hpp>

namespace fpp {

    class Decoder : public FrameProducer {

    public:

        Decoder(const IOParams params);
        virtual ~Decoder() override;

        virtual void        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual Code        processInputData(Packet input_data) override;
        virtual bool        equalTo(const SharedProcessor other) const override;

        const DecoderContext* decoderContext() const;

    private:

        virtual void        onStop() override;

    private:

        DecoderContext      _decoder_context;

    };

    using DecoderList = std::list<Decoder*>;

} // namespace fpp
