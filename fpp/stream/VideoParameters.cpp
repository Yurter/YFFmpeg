#include "VideoParameters.hpp"
#include <fpp/core/Utils.hpp>

namespace ffmpeg { extern "C" {
    #include <libavformat/avformat.h>
} } // namespace ffmpeg

#define DEFAULT_PIXEL_FORMAT    ffmpeg::AV_PIX_FMT_NONE
#define not_inited_pix_fmt(x)   ((x) == DEFAULT_PIXEL_FORMAT)

namespace fpp {

    VideoParameters::VideoParameters()
        : _width { 0 }
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

    void VideoParameters::setAspectRatio(ffmpeg::AVRational aspect_ratio) {
        _aspect_ratio = aspect_ratio;
    }

    void VideoParameters::setFrameRate(ffmpeg::AVRational frame_rate) {
        if ((frame_rate.num * frame_rate.den) == 0) {
            log_error("setFrameRate failed: " << frame_rate);
            ffmpeg::AVRational default_framerate = { 16, 1 };
            log_error("seted default value: " << default_framerate);
            _frame_rate = default_framerate;
            return;
        }
        _frame_rate = frame_rate;
    }

    void VideoParameters::setPixelFormat(ffmpeg::AVPixelFormat pixel_format) {
        if (not_inited_ptr(_codec)) {
            log_error("Cannot set pixel format before codec");
            return;
        }
        if_not(utils::compatible_with_pixel_format(_codec, pixel_format)) { //TODO ? почему формат захардкожен? _codec->pix_fmts[0] ? 14.01
            const auto defailt_h264_pixel_format = ffmpeg::AV_PIX_FMT_YUV420P;
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

    ffmpeg::AVRational VideoParameters::aspectRatio() const {
        return _aspect_ratio;
    }

    ffmpeg::AVRational VideoParameters::frameRate() const {
        return _frame_rate;
    }

    ffmpeg::AVPixelFormat VideoParameters::pixelFormat() const {
        return _pixel_format;
    }

    int64_t VideoParameters::gopSize() const {
        return _gop_size;
    }

    std::string VideoParameters::toString() const {
        return Parameters::toString() + "; "
            + std::to_string(width()) + "x" + std::to_string(height()) + ", "
            + utils::to_string(aspectRatio()) + ", "
            + utils::to_string(frameRate()) + " fps" + ", "
            + utils::to_string(pixelFormat());
    }

    void VideoParameters::completeFrom(const SharedParameters other_params) {
        Parameters::completeFrom(other_params);
        auto other_video_parames = utils::makeVideoParams(other_params);
        if (not_inited_int(width()))            { setWidth(other_video_parames->width());               }
        if (not_inited_int(height()))           { setHeight(other_video_parames->height());             }
        if (not_inited_q(aspectRatio()))        { setAspectRatio(other_video_parames->aspectRatio());   }
        if (not_inited_q(frameRate()))          { setFrameRate(other_video_parames->frameRate());       }
        if (not_inited_pix_fmt(pixelFormat()))  { setPixelFormat(other_video_parames->pixelFormat());   }
    }

    void VideoParameters::parseStream(const ffmpeg::AVStream* avstream) {
        Parameters::parseStream(avstream);
        setWidth(avstream->codecpar->width);
        setHeight(avstream->codecpar->height);
        setAspectRatio(avstream->codecpar->sample_aspect_ratio);
        setFrameRate(avstream->avg_frame_rate);
        setPixelFormat(ffmpeg::AVPixelFormat(avstream->codecpar->format));
        setGopSize(avstream->codec->gop_size);
    }

    void VideoParameters::initStream(ffmpeg::AVStream* avstream) const {
        Parameters::initStream(avstream);
        avstream->codecpar->width = int(width());
        avstream->codecpar->height = int(height());
        avstream->avg_frame_rate = frameRate();
    }

    bool VideoParameters::betterThen(const SharedParameters& other) {
        auto other_vparams = utils::makeVideoParams(other);
        auto this_picture_size = width() * height();
        auto other_picture_size = other_vparams->width() * other_vparams->height();
        return this_picture_size > other_picture_size;
    }

} // namespace fpp