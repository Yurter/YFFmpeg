#include "Frame.hpp"
#include "core/utils.hpp"
#include <atomic>

namespace fpp {

    Frame::Frame() :
        Data<AVFrame>()
    {
        EMPTY_CONSTRUCTOR
    }

    Frame::Frame(const Frame& other) {
        setName("Frame");
        av_frame_ref(&_data, &other._data);
        setType(other.type());
        setInited(true);
    }

    Frame::Frame(const Frame&& other) { //TODO нужен ли?
        UNUSED(other);
        log_error("MOVE FRAME");
    }

    Frame::~Frame() {
        av_frame_unref(&_data);
    }

    Frame& Frame::operator=(const Frame&& other) {
        av_frame_ref(&_data, &other._data);
        setType(other.type());
        setInited(true);
        return *this;
    }

    uint64_t Frame::size() const {
        if (isVideo()) {
            return uint64_t(av_image_get_buffer_size(AVPixelFormat(_data.format), _data.width, _data.height, 32));
        }
        if (isAudio()) {
            auto channels = av_get_channel_layout_nb_channels(_data.channel_layout);
            auto bufer_size = av_samples_get_buffer_size(nullptr/*_data.linesize*/ //TODO разобраться с параметром
                                                         , channels
                                                         , _data.nb_samples
                                                         , AVSampleFormat(_data.format)
                                                         , 32);
            return uint64_t(bufer_size);
        }
        return 0;
    }

//    Code Frame::init() //TODO
//    {
//        return Code::ERR;
//    }

    std::string Frame::toString() const {
        /* Video frame: 33123 bytes, dts 460, pts 460, duration 33 */
        std::string str = utils::media_type_to_string(type()) + " frame: ";
        if (isVideo()) {
            str += std::to_string(size()) + " bytes, "
                    + "pts " + utils::pts_to_string(_data.pts) + ", "
                    + "key_frame " + utils::bool_to_string(_data.key_frame) + ", "
                    + "width " + std::to_string(_data.width) + ", "
                    + "height " + std::to_string(_data.height) + ", "
                    + "px_fmt " + std::string(av_get_pix_fmt_name(AVPixelFormat(_data.format)));
            return str;
        }
        /* Audio frame: 316 bytes, dts 460, pts 460, duration 33 */
        if (isAudio()) {
            str += std::to_string(size()) + " bytes, "
                    + "pts " + utils::pts_to_string(_data.pts) + ", "
                    + "nb_samples " + std::to_string(_data.nb_samples) + ", "
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

} // namespace fpp
