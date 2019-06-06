#include "YRescaler.h"

YVideoRescaler::YVideoRescaler()
{
    //
}

bool YVideoRescaler::init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context)
{
    return false;
}

bool YVideoRescaler::rescale(AVFrame *frame)
{
    return false;
}
