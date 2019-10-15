#include "Frame.hpp"
#include "utils.hpp"

namespace fpp {

    Frame::Frame() :
        Frame(nullptr)
    {
        EMPTY_CONSTRUCTOR
    }

    Frame::Frame(AVFrame *frame) :
        Data<AVFrame*>(frame)//,
    //    _stream_index(INVALID_INT)
    {
        setName("Frame");
    //    _data = frame;
        if (not_inited_ptr(_data)) { alloc(); }
    //    if (_data == nullptr) { alloc(); }
    }

    Frame::~Frame() {
        // TODO ffmpeg ф-ии на отчистку
    //    if (_data != nullptr) {
    //        av_frame_free(&_data);
    //    }
    }

    bool Frame::alloc() //TODO
    {
        _data = av_frame_alloc();
        if (_data == nullptr) { return false; }
        return true;
    }

    void Frame::free() {
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

    std::string Frame::toString() const
    {
        /* Video frame: 33123 byte, dts 460, pts 460, duration 33 */
        /* Audio frame: 316 byte, dts 460, pts 460, duration 33   */
        std::string str = utils::media_type_to_string(type()) + " frame: ";
        if (isVideo()) {
//            str += std::to_string(_data->linesize[0]) + " bytes, "
            int size = av_image_get_buffer_size(AVPixelFormat(_data->format), _data->width, _data->height, 32);
            str += std::to_string(size) + " bytes, "
                    + "pts " + utils::pts_to_string(_data->pts) + ", "
                    + "key_frame " + std::to_string(_data->key_frame) + ", "
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
