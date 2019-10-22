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
        void                setFrameRate(float frame_rate);
        void                setFrameRate(AVRational frame_rate);
        void                setPixelFormat(AVPixelFormat pixel_format);

        int64_t             width()         const;
        int64_t             height()        const;
        AVRational          aspectRatio()   const;
        float               frameRate()     const;
        AVPixelFormat       pixelFormat()   const;

        virtual std::string toString() const override;

    //    void                softCopy(VideoParameters* other_parametrs);
        VideoParameters&   operator=(const VideoParameters& rhs);

    private:

        int64_t             _width;
        int64_t             _height;
        AVRational          _aspect_ratio;
        float               _frame_rate;
        AVPixelFormat       _pixel_format;

    };

} // namespace fpp
