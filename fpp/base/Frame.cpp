#include "Frame.hpp"
#include <fpp/core/Utils.hpp>

namespace ffmpeg { extern "C" {
    #include <libavutil/imgutils.h>
} } // namespace ffmpeg

namespace fpp {

    Frame::Frame()
        : Data<ffmpeg::AVFrame>() {
        setName("Frame");
    }

    Frame::Frame(const Frame& other)
        : Frame() {
        copy(other);
    }

    Frame::Frame(const ffmpeg::AVFrame& frame, MediaType type)
        : Frame() {
        copy(frame, type);
    }

    Frame::~Frame() {
        av_frame_unref(&_data);
    }

    Frame& Frame::operator=(const Frame& other) {
        copy(other);
        return *this;
    }

    int64_t Frame::pts() const {
        return _data.pts;
    }

    void Frame::setPts(int64_t value) {
        _data.pts = value;
    }

    bool Frame::keyFrame() const {
        return _data.key_frame == 1;
    }

    uint64_t Frame::size() const {
        if (isVideo()) {
            return uint64_t(ffmpeg::av_image_get_buffer_size(ffmpeg::AVPixelFormat(_data.format), _data.width, _data.height, 32));
        }
        if (isAudio()) {
            auto channels = ffmpeg::av_get_channel_layout_nb_channels(_data.channel_layout);
            auto bufer_size = ffmpeg::av_samples_get_buffer_size(nullptr
                                                         , channels
                                                         , _data.nb_samples
                                                         , ffmpeg::AVSampleFormat(_data.format)
                                                         , 32);
            return uint64_t(bufer_size);
        }
        return 0;
    }

    std::string Frame::toString() const {
        /* Video frame: 460800 bytes, pts 1016370, key_frame false, width 640, height 480, yuv420p */
        std::string str = utils::to_string(type()) + " frame: ";
        if (isVideo()) {
            str += std::to_string(size()) + " bytes, "
                    + (keyFrame() ? "[I]" : "[_]") + ", "
                    + "pts " + utils::pts_to_string(_data.pts) + ", "
                    + "width " + std::to_string(_data.width) + ", "
                    + "height " + std::to_string(_data.height) + ", "
                    + utils::to_string(ffmpeg::AVPixelFormat(_data.format));
            return str;
        }
        /* Audio frame: 1024 bytes, pts 425984, nb_samples 1024, channel_layout 4, sample_rate 44100 */
        if (isAudio()) {
            str += std::to_string(size()) + " bytes, "
                    + (keyFrame() ? "[I]" : "[_]") + ", "
                    + "pts " + utils::pts_to_string(_data.pts) + ", "
                    + "samples " + std::to_string(_data.nb_samples) + ", "
                    + "channel_layout " + std::to_string(_data.channel_layout) + ", "
                    + "sample_rate " + std::to_string(_data.sample_rate);
            return str;
        }
        /* Unknown frame: -1 bytes */
        {
            str += std::to_string(-1) + " bytes";
            return str;
        }
    }

    void Frame::copy(const Frame& other) {
        av_frame_ref(&_data, &other._data);
        setType(other.type());
        setInited(true);
    }

    void Frame::copy(const ffmpeg::AVFrame& other, MediaType type) {
        av_frame_ref(&_data, &other);
        setType(type);
        setInited(true);
    }

} // namespace fpp
