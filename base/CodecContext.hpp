#pragma once
#include "TemplateProcessor.hpp"
#include "base/Stream.hpp"

namespace fpp {

    class CodecContext : public Object {

    public:

        CodecContext(Stream* stream, CodecType type);
        virtual ~CodecContext() override;

        virtual Code        initParams() = 0;
        virtual Code        init() override;

        Code                open();
        Code                close();

        bool                opened() const;
        bool                closed() const;

        std::string         toString() const override final;

        AVCodecContext*     codecContext();

    private:

        void                setOpened(bool value);

    protected:

        Stream*             _stream;
        AVCodecContext*     _codec_context;
        CodecType           _type;
        bool                _opened;

    };

} // namespace fpp
