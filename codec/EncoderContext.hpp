#pragma once
#include "base/CodecContext.hpp"
#include "base/Frame.hpp"
#include "base/Packet.hpp"

namespace fpp {

    class EncoderContext : public CodecContext {

    public:

        EncoderContext(const IOParams params);
        virtual ~EncoderContext() override;

        Code                encode(Frame input_frame, Packet& output_packet); //TODO rename 'output_packet' to 'encoded_packet' 10.01
        virtual Code        flush(Object* data) override;
        virtual Code        onOpen()            override;

    private:

        virtual Code        initParams()    override;

    };

    using EncoderContextList = std::list<EncoderContext*>;

} // namespace fpp
