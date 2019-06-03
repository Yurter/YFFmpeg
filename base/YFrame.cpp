#include "YFrame.h"

YFrame::YFrame() :
    YData<AVFrame>()
{
    //
}

YFrame::~YFrame()
{
    // TODO ffmpeg ф-ии на отчистку
    //av_frame_free(&m_raw);
}

bool YFrame::alloc() //TODO
{
    AVFrame* frame;
    frame = av_frame_alloc();
    if (frame == nullptr) { return false; }
    _data = *frame;
    return true;
}

bool YFrame::empty() const //TODO frame sizes...
{
    switch (_type) {
    case MEDIA_TYPE_VIDEO:
        return _data.linesize[0];
    case MEDIA_TYPE_AUDIO:
        return _data.nb_samples;
    case MEDIA_TYPE_UNKNOWN:
        return true;
    }
}

std::string YFrame::toString() const
{
    return "TODO"; //TODO
}
