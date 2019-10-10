#pragma once
#include "DataProcessor.hpp"
#include "Frame.hpp"

namespace fpp {

    class Filter : public DataProcessor<Frame,Frame> {

    public:

        Filter();

    };

} // namespace fpp
