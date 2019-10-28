#pragma once
#include "TemplateProcessor.hpp"
#include "base/Stream.hpp"

namespace fpp {

    class CodecContext : public Object {

    public:

        CodecContext(Stream* stream, CodecType type);
        virtual ~CodecContext() override;

        virtual Code initParams() = 0;
        virtual Code init() override;

        Code open();
        Code close();

        std::string toString() const override final;

        AVCodecContext* codecContext();

    protected:

        Stream*             _stream;
        AVCodecContext*     _codec_context;
        CodecType           _type;

    };

} // namespace fpp
