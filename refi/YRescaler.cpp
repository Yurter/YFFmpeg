#include "YRescaler.h"

YRescaler::YRescaler(streams_pair audio_streams) :
    YAbstractRefi(audio_streams)
{
    setName("YRescaler");
}

YRescaler::~YRescaler()
{
    // TODO
}

YCode YRescaler::init()
{
    return YCode::ERR;
}

YCode YRescaler::processInputData(YFrame& input_data)
{
    return YCode::NOT_INITED;
}
