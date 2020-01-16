#include "VideoParameters.hpp"
#include "core/utils.hpp"

namespace fpp {

    VideoParameters::VideoParameters(ParamsType type)
        : Parameters(type)
        , _width { 0 }
        , _height { 0 }
        , _aspect_ratio { DEFAULT_RATIONAL }
        , _frame_rate { DEFAULT_RATIONAL }
        , _pixel_format { DEFAULT_PIXEL_FORMAT }
        , _gop_size { 0 } {
        setName("VideoParameters");
        setType(MediaType::Video);
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
        if_not(utils::compatible_with_pixel_format(_codec, pixel_format)) { //TODO ? почему формат захардкожен? _codec->pix_fmts[0] ? 14.01
            const auto defailt_h264_pixel_format = AV_PIX_FMT_YUV420P;
            log_warning("Cannot set pixel format: " << pixel_format
                        << " - " << _codec->name << " doesn't compatible with it, "
                        << "setting default: " << defailt_h264_pixel_format);
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
        return Parameters::toString() + "; "
            + std::to_string(width()) + "x" + std::to_string(height()) + ", "
            + utils::rational_to_string(aspectRatio()) + ", "
            + utils::rational_to_string(frameRate()) + " fps" + ", "
            + utils::pixel_format_to_string(pixelFormat());
    }

    Code VideoParameters::completeFrom(const ParametersPointer other_params) {
        auto other_video_parames = std::static_pointer_cast<const VideoParameters>(other_params);
        if (not_inited_int(_width))             { setWidth(other_video_parames->width());                }
        if (not_inited_int(_height))            { setHeight(other_video_parames->height());              }
        if (not_inited_q(_aspect_ratio))        { setAspectRatio(other_video_parames->aspectRatio());    }
        if (not_inited_q(_frame_rate))          { setFrameRate(other_video_parames->frameRate());        }
        if (not_inited_pix_fmt(_pixel_format))  { setPixelFormat(other_video_parames->pixelFormat());    }
        try_to(Parameters::completeFrom(other_params));
        return Code::OK;
    }

    void VideoParameters::parseStream(const AVStream* avstream) {
        Parameters::parseStream(avstream);
        setWidth(avstream->codecpar->width);
        setHeight(avstream->codecpar->height);
        setAspectRatio(avstream->codecpar->sample_aspect_ratio);
        setFrameRate(avstream->avg_frame_rate);
        setPixelFormat(avstream->codec->pix_fmt);
        setGopSize(avstream->codec->gop_size);
    }

    bool VideoParameters::betterThen(const ParametersPointer& other) {
        auto other_vparams = std::static_pointer_cast<const VideoParameters>(other);
        auto this_picture_size = width() * height();
        auto other_picture_size = other_vparams->width() * other_vparams->height();
        return this_picture_size > other_picture_size;
    }

} // namespace fpp
