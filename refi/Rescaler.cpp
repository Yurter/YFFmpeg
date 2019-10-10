#include "Rescaler.hpp"

namespace fpp {

    Rescaler::Rescaler(StreamPair audio_streams) :
        Refi(audio_streams)
    {
        setName("Rescaler");
    }

    Rescaler::~Rescaler()
    {
        // TODO
    }

    Code Rescaler::init()// TODO
    {
        return Code::ERR;
    }

    Code Rescaler::processInputData(Frame& input_data)// TODO
    {
        return Code::NOT_INITED;
    }

} // namespace fpp
