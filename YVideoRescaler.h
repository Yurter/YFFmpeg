#pragma once
#include "ffmpeg.h"

class YVideoRescaler
{

public:

    YVideoRescaler();

    bool rescale(AVFrame* frame);

};
