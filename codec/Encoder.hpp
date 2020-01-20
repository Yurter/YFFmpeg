#pragma once
#include "inout/PacketProducer.hpp"
#include "EncoderContext.hpp"

namespace fpp {

    class Encoder : public PacketProducer {

    public:

        Encoder(const IOParams params);
        virtual ~Encoder() override;

        virtual Code        init() override;
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
