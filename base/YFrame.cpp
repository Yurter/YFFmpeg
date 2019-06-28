#include "YFrame.h"
#include "utils.h"

YFrame::YFrame() :
    YFrame(nullptr)
{
    //
}

YFrame::YFrame(AVFrame *frame) :
    YData<AVFrame*>()
{
    setName("YFrame");
    _data = frame;
    if (_data == nullptr) { alloc(); }
}

YFrame::~YFrame()
{
    // TODO ffmpeg ф-ии на отчистку
//    if (_data != nullptr) {
//        av_frame_free(&_data);
//    }
}

bool YFrame::alloc() //TODO
{
    _data = av_frame_alloc();
    if (_data == nullptr) { return false; }
    return true;
}

void YFrame::free()
{
    av_frame_free(&_data);
}

bool YFrame::empty() const //TODO frame sizes...
{
    switch (_type) {
    case MEDIA_TYPE_VIDEO:
        return _data->linesize[0];
    case MEDIA_TYPE_AUDIO:
        return _data->nb_samples;
    case MEDIA_TYPE_UNKNOWN:
        return true;
    }
}

YCode YFrame::init()
{
    return YCode::ERR; //TODO
}

std::string YFrame::toString() const //TODO
{
    /* Video frame: 33123 byte, dts 460, pts 460, duration 33 */
    /* Audio frame: 316 byte, dts 460, pts 460, duration 33 */
    auto pts_str = _data->pts == AV_NOPTS_VALUE ? "NOPTS" : std::to_string(_data->pts);
    std::string str = utils::media_type_to_string(_type) + " frame: ";
    if (isVideo()) {
        str += std::to_string(_data->linesize[0]) + " bytes, "
                + "pts " + pts_str + ", "
                + "key_frame " + std::to_string(_data->key_frame) + ", "
                + "width " + std::to_string(_data->width) + ", "
                + "height " + std::to_string(_data->height);
    }
    if (isAudio()) {
        str += std::to_string(_data->linesize[0]) + " bytes, "
                + "pts " + pts_str + ", "
                + "nb_samples " + std::to_string(_data->nb_samples) + ", "
                + "channel_layout " + std::to_string(_data->channel_layout) + ", "
                + "sample_rate " + std::to_string(_data->sample_rate);
    }
    return str;
}
