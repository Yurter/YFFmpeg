#pragma once
#include "TemplateProcessor.hpp"
#include "../base/Stream.hpp"
#include "../base/Frame.hpp"
#include "../base/Packet.hpp"

namespace fpp {

    class Codec : public Processor {

    public:

        Codec(Stream* stream, CodecType type);

        virtual ~Codec() override;

        virtual Code init() override;
        Code open();
        Code close();

        std::string toString() const override final;

        AVCodecContext* codecContext();

    protected:

        Stream*             _stream;
        AVCodecContext*     _codec_context;
        CodecType           _type;


        // Processor interface
    public:
    };

} // namespace fpp
