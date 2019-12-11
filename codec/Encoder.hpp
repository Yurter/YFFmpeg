#pragma once
#include "inout/PacketProducer.hpp"
#include "EncoderContext.hpp"

namespace fpp {

    class Encoder : public PacketProducer {

    public:

        Encoder(const IOParams params);

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual Code        processInputData(Frame input_data) override;
        virtual bool        equalTo(const ProcessorPointer other) const override;

        const EncoderContext* encoderContext() const;

    private:

        EncoderContext      _encoder_context;

    };

    using EncoderList = std::list<Encoder*>;

} // namespace fpp
