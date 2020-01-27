#include "FrameSink.hpp"

namespace fpp {

    FrameSink::FrameSink()
    {
        setName("FrameSink");
        try_throw(setPostFunction(std::bind(&FrameSink::writeFrame, this)));
    }

    FrameSink::~FrameSink() {
        // TODO
    }

    Code FrameSink::writeFrame() {
        Frame frame;
        try_to(restoreOutputData(frame));
        try_to(writeOutputData(frame));
        return Code::OK;
    }

} // namespace fpp
