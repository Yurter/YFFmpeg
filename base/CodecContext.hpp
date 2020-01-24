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

        CodecContext(const IOParams params, CodecType type); //TODO оставить только один из параметров, зачем оба ? 24.01
        virtual ~CodecContext() override;

        virtual Code        flush(Object* data) = 0; //TODO (отправлять кодеку NULL)
        virtual Code        init() override;
        std::string         toString() const override final;

        bool                opened() const;
        bool                closed() const;

        SharedAVCodecContext            context();
        virtual const ffmpeg::AVCodec*  codec() = 0;
        virtual SharedParameters        parameters() = 0;

        const IOParams      params;

    private:

        Code                open();
        Code                close();
        void                setOpened(bool value);

    protected:

        SharedAVCodecContext    _codec_context;

    private:

        CodecType               _type;
        bool                    _opened;

    };

} // namespace fpp
