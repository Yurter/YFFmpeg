#include "VideoParameters.hpp"
#include "core/utils.hpp"

namespace fpp {

    VideoParameters::VideoParameters() :
        Parameters(MediaType::MEDIA_TYPE_VIDEO)
        , _width(DEFAULT_INT)
        , _height(DEFAULT_INT)
        , _aspect_ratio(DEFAULT_RATIONAL)
        , _frame_rate(DEFAULT_RATIONAL)
        , _pixel_format(DEFAULT_PIXEL_FORMAT)
        , _gop_size(INVALID_INT)
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

//    void VideoParameters::setFrameRate(float frame_rate) { //TODO сделать AVRational { frame_rate, 1 }, удалить?
//        _frame_rate = frame_rate;
//    }

    void VideoParameters::setFrameRate(AVRational frame_rate) {
        if ((frame_rate.num * frame_rate.den) == 0) {
            log_error("setFrameRate failed: " << utils::rational_to_string(frame_rate));
            AVRational default_framerate = { 16, 1 };
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
        if_not(utils::compatible_with_pixel_format(_codec, pixel_format)) {
            const auto defailt_h264_pixel_format = AV_PIX_FMT_YUV420P;
            log_warning("Cannot set pixel format: " << av_get_pix_fmt_name(pixel_format)
                        << " - " << _codec->name << " doesn't compatible with it, "
                        << "setting default: " << av_get_pix_fmt_name(defailt_h264_pixel_format));
            _pixel_format = defailt_h264_pixel_format;
            return;
        }
        _pixel_format = pixel_format;
    }

    void VideoParameters::setGopSize(int64_t value) {
        _gop_size = value;
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

    int64_t VideoParameters::gopSize() const {
        return _gop_size;
    }

    std::string VideoParameters::toString() const {
        std::string str = Parameters::toString() + "; ";
        str += std::to_string(width()) + "x" + std::to_string(height()) + ", ";
        str += utils::rational_to_string(aspectRatio()) + ", ";
        str += utils::rational_to_string(frameRate()) + " fps" + ", ";
        str += utils::pixel_format_to_string(pixelFormat());
        return str;
    }

    Code VideoParameters::completeFrom(const Parameters* other_parametrs) {
        auto other_video_parameters = static_cast<const VideoParameters*>(other_parametrs);
        if (not_inited_int(_width))             { setWidth(other_video_parameters->width());                }
        if (not_inited_int(_height))            { setHeight(other_video_parameters->height());              }
        if (not_inited_q(_aspect_ratio))        { setAspectRatio(other_video_parameters->aspectRatio());    }
        if (not_inited_q(_frame_rate))          { setFrameRate(other_video_parameters->frameRate());        }
        if (not_inited_pix_fmt(_pixel_format))  { setPixelFormat(other_video_parameters->pixelFormat());    }
        try_to(Parameters::completeFrom(other_parametrs));
        return Code::OK;
    }

} // namespace fpp
