#pragma once
#include <fpp/inout/PacketProducer.hpp>
#include <fpp/codec/EncoderContext.hpp>

namespace fpp {

    class Encoder : public PacketProducer {

    public:

        Encoder(const SharedParameters parameters);
        virtual ~Encoder() override;

        virtual void        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual Code        processInputData(Frame input_data) override;
        virtual bool        equalTo(const SharedProcessor other) const override;

        const EncoderContext* encoderContext() const;

    private:

        virtual void        onStop() override;

    private:

        EncoderContext      _encoder_context;

    };

    using EncoderList = std::list<Encoder*>;

} // namespace fpp
