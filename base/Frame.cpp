#include "Frame.hpp"
#include "core/utils.hpp"
#include <atomic>

namespace fpp {

    Frame::Frame() :
        Frame(nullptr)
    {
        EMPTY_CONSTRUCTOR
    }

    Frame::Frame(const Frame& other) {
        setName("Frame");
        if (not_inited_ptr(_data)) {
            alloc();
        }
        av_frame_ref(_data, other._data);
        setType(other.type());
        setInited(true);
//        log_error("Copy constructor " << other.toString());
    }

    Frame::Frame(const Frame&& other) {
        log_error("MOVE FRAME");
    }

    Frame::Frame(AVFrame *frame) :
        Data<AVFrame*>(frame)//,
    //    _stream_index(INVALID_INT)
    {
        setName("Frame");
    //    _data = frame;
        if (not_inited_ptr(_data)) { alloc(); }

//        static std::atomic_int memory_leak = 0;
//        memory_leak += sizeof (*_data);
//        log_warning("Allocated: " << memory_leak);
    //    if (_data == nullptr) { alloc(); }
    }

    Frame::~Frame() {
        av_frame_unref(_data);
    }

    Frame& Frame::operator=(const Frame&& other) {
        if (not_inited_ptr(_data)) {
            alloc();
        }
        av_frame_ref(_data, other._data);
        setType(other.type());
        setInited(true);
        return *this;
    }

    bool Frame::alloc() //TODO
    {
        _data = av_frame_alloc();
        if (_data == nullptr) { return false; }
        return true;
    }

    void Frame::free() {
//        av_frame_unref(_data); ??
        av_frame_free(&_data);
    }

    bool Frame::empty() const {
        if (isVideo()) {
            return _data->linesize[0] == 0;
        }
        if (isAudio()) {
            return _data->nb_samples == 0;
        }
        return true;
    }

    Code Frame::init() //TODO
    {
        return Code::ERR;
    }

    std::string Frame::toString() const {
        /* Video frame: 33123 byte, dts 460, pts 460, duration 33 */
        /* Audio frame: 316 byte, dts 460, pts 460, duration 33   */
        std::string str = utils::media_type_to_string(type()) + " frame: ";
        if (isVideo()) {
            int size = av_image_get_buffer_size(AVPixelFormat(_data->format), _data->width, _data->height, 32);
            str += std::to_string(size) + " bytes, "
                    + "pts " + utils::pts_to_string(_data->pts) + ", "
                    + "key_frame " + utils::bool_to_string(_data->key_frame) + ", "
                    + "width " + std::to_string(_data->width) + ", "
                    + "height " + std::to_string(_data->height);
        }
        if (isAudio()) {
            str += std::to_string(_data->linesize[0]) + " bytes, "//TODO размеры фреймов
                    + "pts " + utils::pts_to_string(_data->pts) + ", "
                    + "nb_samples " + std::to_string(_data->nb_samples) + ", "
                    + "channel_layout " + std::to_string(_data->channel_layout) + ", "
                    + "sample_rate " + std::to_string(_data->sample_rate);
        }
        return str;
    }

} // namespace fpp
