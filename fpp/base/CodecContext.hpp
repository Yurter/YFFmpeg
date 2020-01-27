#pragma once
#include <fpp/base/Parameters.hpp>

struct AVCodecContext;

namespace fpp {

    using SharedAVCodecContext = std::shared_ptr<AVCodecContext>;

    class CodecContext : public Object {

    public:

        CodecContext(const SharedParameters parameters);
        virtual ~CodecContext() override;

        virtual Code        flush(Object* data) = 0; //TODO (отправлять кодеку NULL)
        virtual void        init() override;
        std::string         toString() const override final;

        bool                opened() const;
        bool                closed() const;

        SharedAVCodecContext    context();
        const AVCodec*          codec();
        virtual Code        onOpen() { return Code::OK; } //TODO убрать 24.01

//        const IOParams      params;
        const SharedParameters  params;

    private:

        Code                open();
        Code                close();
        void                setOpened(bool value);

    private:

        SharedAVCodecContext    _codec_context;
        bool                    _opened;

    };

} // namespace fpp
