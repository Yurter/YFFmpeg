#include "YFrame.h"

YFrame::YFrame() :
    YData<AVFrame*>()
{
//    _data = av_frame_alloc();
    alloc();
}

YFrame::YFrame(AVFrame *frame)
{
    _data = frame;
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

std::string YFrame::toString() const
{
    return "TODO"; //TODO
}
