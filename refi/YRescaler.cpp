#include "YRescaler.hpp"

YRescaler::YRescaler(streams_pair audio_streams) :
    YRefi(audio_streams)
{
    setName("YRescaler");
}

YRescaler::~YRescaler()
{
    // TODO
}

YCode YRescaler::init()// TODO
{
    return YCode::ERR;
}

YCode YRescaler::processInputData(YFrame& input_data)// TODO
{
    return YCode::NOT_INITED;
}
