#include "VideoParameters.hpp"
#include "core/utils.hpp"

namespace fpp {

    VideoParameters::VideoParameters() :
        Parameters(MediaType::MEDIA_TYPE_VIDEO),
        _width(DEFAULT_INT),
        _height(DEFAULT_INT),
        _aspect_ratio(DEFAULT_RATIONAL),
        _frame_rate(DEFAULT_RATIONAL),
        _pixel_format(DEFAULT_PIXEL_FORMAT)
    {
        setName("VideoParameters");
    }

    //VideoParameters::VideoParameters(Parameters parameters) :
    //    Parameters(parameters),
    //    _width(DEFAULT_INT),
    //    _height(DEFAULT_INT),
    //    _aspect_ratio(DEFAULT_RATIONAL),
    //    _frame_rate(DEFAULT_FLOAT),
    //    _pixel_format(DEFAULT_PIXEL_FORMAT)
    //{
    //    setName("VideoParameters");
    //}

    VideoParameters::~VideoParameters() {
        //
    }

    void VideoParameters::setWidth(int64_t width) {
        _width = width - (width % 2);
    }

    void VideoParameters::setHeight(int64_t height) {
        _height = height - (height % 2);
    }

    void VideoParameters::setAspectRatio(AVRational aspect_ratio) {
        _aspect_ratio = aspect_ratio;
    }

//    void VideoParameters::setFrameRate(float frame_rate) {
//        _frame_rate = frame_rate;
//    }

    void VideoParameters::setFrameRate(AVRational frame_rate) {
        if ((frame_rate.num * frame_rate.den) == 0) {
            log_error("setFrameRate failed: " << utils::rational_to_string(frame_rate));
            AVRational default_framerate = { 25, 1 };
            log_error("seted default value: " << utils::rational_to_string(default_framerate));
            _frame_rate = default_framerate;
            return;
        }
        _frame_rate = frame_rate;
    }

    void VideoParameters::setPixelFormat(AVPixelFormat pixel_format) {
        if (not_inited_ptr(_codec)) {
            log_error("Cannot set pixel format before codec");
            return;
        }
        AVPixelFormat h264_pxl_fmts[] = {
            AV_PIX_FMT_YUV420P
            , AV_PIX_FMT_YUVJ420P
            , AV_PIX_FMT_YUV422P
            , AV_PIX_FMT_YUVJ422P
            , AV_PIX_FMT_YUV444P
            , AV_PIX_FMT_YUVJ444P
            , AV_PIX_FMT_NV12
            , AV_PIX_FMT_NV16
            , AV_PIX_FMT_NV21
            , AV_PIX_FMT_YUV420P10LE
            , AV_PIX_FMT_YUV422P10LE
            , AV_PIX_FMT_YUV444P10LE
            , AV_PIX_FMT_NV20LE
            , AV_PIX_FMT_GRAY8
            , AV_PIX_FMT_GRAY10LE
        };
        if_not(utils::compatible_with_pixel_format(_codec, pixel_format)) {
            log_warning("Cannot set pixel format: " << av_get_pix_fmt_name(pixel_format)
                        << " - " << _codec->name << " doesn't compatible with it, "
                        << "setting default: " << av_get_pix_fmt_name(_codec->pix_fmts[0]));
            _pixel_format = _codec->pix_fmts[0];
            return;
        }
        _pixel_format = pixel_format;
    }

    int64_t VideoParameters::width() const {
        return _width;
    }

    int64_t VideoParameters::height() const {
        return _height;
    }

    AVRational VideoParameters::aspectRatio() const {
        return _aspect_ratio;
    }

    AVRational VideoParameters::frameRate() const {
        return _frame_rate;
    }

    AVPixelFormat VideoParameters::pixelFormat() const {
        return _pixel_format;
    }

    std::string VideoParameters::toString() const {
        std::string str = "TODO";
        return str;
    }

    Code VideoParameters::completeFrom(const Parameters* other_parametrs) { //TODO заменить все присваивания на сеттеры
        auto other_video_parameters = static_cast<const VideoParameters*>(other_parametrs);
        if (not_inited_int(_width))             { _width = other_video_parameters->width();                 }
        if (not_inited_int(_height))            { _height = other_video_parameters->height();               }
        if (not_inited_q(_aspect_ratio))        { _aspect_ratio = other_video_parameters->aspectRatio();    }
        if (not_inited_q(_frame_rate))          { _frame_rate = other_video_parameters->frameRate();        }
//        if (not_inited_pix_fmt(_pixel_format))  { _pixel_format = other_video_parameters->pixelFormat();    }
        if (not_inited_pix_fmt(_pixel_format))  { setPixelFormat(other_video_parameters->pixelFormat());    }
        try_to(Parameters::completeFrom(other_parametrs));
        return Code::OK;
    }

    //void VideoParameters::toCodecpar(AVCodecParameters* codecpar)
    //{
    //    codecpar->width = int(_width);
    //    codecpar->height = int(_height);
    //    Parameters::toCodecpar(codecpar);
    //}

    //void VideoParameters::softCopy(VideoParameters &other_parametrs)
    //{
    //    if (not_inited_int(_width))             { _width = other_parametrs.width();                 }
    //    if (not_inited_int(_height))            { _height = other_parametrs.height();               }
    //    if (not_inited_q(_aspect_ratio))        { _aspect_ratio = other_parametrs.aspectRatio();    }
    //    if (not_inited_float(_frame_rate))      { _frame_rate = other_parametrs.frameRate();        }
    //    if (not_inited_pix_fmt(_pixel_format))  { _pixel_format = other_parametrs.pixelFormat();    }
    //    Parameters::softCopy(other_parametrs);
    //}

    VideoParameters& VideoParameters::operator=(const VideoParameters &rhs) {
        _width          = rhs.width();
        _height         = rhs.height();
        _aspect_ratio   = rhs.aspectRatio();
        _frame_rate     = rhs.frameRate();
        _pixel_format   = rhs.pixelFormat();
        Parameters::operator=(rhs);
        return *this;
    }

} // namespace fpp
