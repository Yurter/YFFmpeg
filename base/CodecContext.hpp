#pragma once
#include <memory>
#include <core/Object.hpp>
#include <base/Parameters.hpp>

namespace ffmpeg {
    struct AVCodecContext;
} // namespace ffmpeg

namespace fpp {

    using SharedAVCodecContext = std::shared_ptr<ffmpeg::AVCodecContext>;

    class CodecContext : public Object {

    public:

        CodecContext(const IOParams params, CodecType type);
        virtual ~CodecContext() override;

        virtual Code        initParams() = 0;
        virtual Code        flush(Object* data) = 0; //TODO (отправлять кодеку NULL)
        virtual Code        init() override;
        std::string         toString() const override final;
        virtual Code        onOpen();

        Code                open();
        Code                close();

        bool                opened() const;
        bool                closed() const;

        SharedAVCodecContext    codecContext();

        const IOParams      params;

    private:

        void                setOpened(bool value);

    protected:

        SharedAVCodecContext    _codec_context;

    private:

        CodecType               _type;
        bool                    _opened;

    };

} // namespace fpp
