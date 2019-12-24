#pragma once
#include "base/Parameters.hpp"

namespace fpp {

    class VideoParameters : public Parameters {

    public:

        VideoParameters();
        virtual ~VideoParameters() override;

        void                setWidth(int64_t width);
        void                setHeight(int64_t height);
        void                setAspectRatio(AVRational aspect_ratio);
        void                setFrameRate(AVRational frame_rate);
        void                setPixelFormat(AVPixelFormat pixel_format);
        void                setGopSize(int64_t value);

        int64_t             width()         const;
        int64_t             height()        const;
        AVRational          aspectRatio()   const;
        AVRational          frameRate()     const;
        AVPixelFormat       pixelFormat()   const;
        int64_t             gopSize()       const;

        virtual std::string toString() const override;

        Code                completeFrom(const Parameters* other_parametrs) override;

    private:

        int64_t             _width;
        int64_t             _height;
        AVRational          _aspect_ratio;
        AVRational          _frame_rate;
        AVPixelFormat       _pixel_format;
        int64_t             _gop_size;

    };

} // namespace fpp
