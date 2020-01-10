#pragma once
#include "TemplateProcessor.hpp"
#include "base/Stream.hpp"

namespace fpp {

    class CodecContext : public Object {

    public:

//        CodecContext(const IOParams params, CodecType type);
        CodecContext(const IOParams params, CodecType type);
        virtual ~CodecContext() override;

        virtual Code        initParams() = 0;
        virtual Code        flush(Object* data) = 0; //TODO (отправлять кодеку NULL)
        virtual Code        init() override;
        virtual Code        onOpen();

        Code                open();
        Code                close();

        bool                opened() const;
        bool                closed() const;

        std::string         toString() const override final;

        AVCodecContext*     codecContext();

        const IOParams      params;

    private:

        void                setOpened(bool value);

    protected:

        AVCodecContext*     _codec_context;
        CodecType           _type;
        bool                _opened;

    };

} // namespace fpp
