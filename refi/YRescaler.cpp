#include "YRescaler.h"

YRescaler::YRescaler()
{
    //
}

bool YRescaler::init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context)
{
    return true;
//    return false;
}

bool YRescaler::rescale(AVFrame *frame)
{
    return false;
}

YCode YRescaler::processInputData(YFrame& input_data)
{
    return YCode::ERR;
}