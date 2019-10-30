#include "Processor.hpp"

namespace fpp {

    Processor::Processor() :
        _next_processor(nullptr)
        , _opened(false)
    {
        setName("Processor");
    }

    Processor::~Processor() {
        //TODO
    }

    Code Processor::open() {
        return Code::NOT_IMPLEMENTED;
    }

    Code Processor::close() {
        return Code::NOT_IMPLEMENTED;
    }

    bool Processor::opened() const {
        return _opened;
    }

    bool Processor::closed() const {
        return !_opened;
    }

    void Processor::setOpened(bool opened) {
        _opened = opened;
    }

    Code fpp::Processor::connectTo(fpp::Processor* other) {
        auto ptr = dynamic_cast<const Processor*>(other);
        return_if(not_inited_ptr(ptr), Code::INVALID_INPUT);
        _next_processor = other;
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
