#pragma once
#include "TemplateProcessor.hpp"
#include "stream/Stream.hpp"

namespace fpp {

    using SharedCodecContext = std::shared_ptr<AVCodecContext>;

    class CodecContext : public Object {

    public:

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

        SharedCodecContext  codecContext();

        const IOParams      params;

    private:

        void                setOpened(bool value);

    protected:

        SharedCodecContext  _codec_context;

    private:

        CodecType           _type;
        bool                _opened;

    };

} // namespace fpp
