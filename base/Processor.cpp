#include "Processor.hpp"

namespace fpp {

    Code fpp::Processor::connectTo(fpp::Processor* other) {
        auto ptr = dynamic_cast<const Processor*>(other);
        return_if(not_inited_ptr(ptr), Code::INVALID_INPUT);
        _next_processor = ptr;
        return Code::OK;
    }

    void Processor::setSkipType(MediaType type) {
        _skip_types |= type;
    }

    bool Processor::skipType(MediaType type) const {
        return _skip_types & type;
    }

    void Processor::setDiscardType(MediaType type) {
        _discard_types |= type;
    }

    bool Processor::discardType(MediaType type) const {
        return _discard_types & type;
    }

} // namespace fpp
