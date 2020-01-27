#pragma once
#include <fpp/base/Parameters.hpp>

namespace fpp {

    class VideoParameters;
    using SharedVideoParameters = std::shared_ptr<VideoParameters>;

    class VideoParameters : public Parameters {

    public:

        VideoParameters();
        virtual ~VideoParameters() override = default;

        void                setWidth(int64_t width);
        void                setHeight(int64_t height);
        void                setAspectRatio(ffmpeg::AVRational aspect_ratio);
        void                setFrameRate(ffmpeg::AVRational frame_rate);
        void                setPixelFormat(ffmpeg::AVPixelFormat pixel_format);
        void                setGopSize(int64_t value);

        int64_t                 width()         const;
        int64_t                 height()        const;
        ffmpeg::AVRational      aspectRatio()   const;
        ffmpeg::AVRational      frameRate()     const;
        ffmpeg::AVPixelFormat   pixelFormat()   const;
        int64_t                 gopSize()       const;

        virtual std::string toString() const override;

        virtual void        completeFrom(const SharedParameters other_params)   override;
        virtual void        parseStream(const ffmpeg::AVStream* avstream)       override;
        virtual void        initStream(ffmpeg::AVStream* avstream) const        override;
        virtual bool        betterThen(const SharedParameters& other)           override;

//        friend SharedVideoParameters make_shared_video_params() {
//            return std::make_shared<VideoParameters>();
//        }

    private:

        int64_t                 _width;
        int64_t                 _height;
        ffmpeg::AVRational      _aspect_ratio;
        ffmpeg::AVRational      _frame_rate;
        ffmpeg::AVPixelFormat   _pixel_format;
        int64_t                 _gop_size;

    };

} // namespace fpp
