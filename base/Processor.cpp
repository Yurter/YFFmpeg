#include "Processor.hpp"
#include "core/utils.hpp"

namespace fpp {

    Processor::Processor(ProcessorType type) :
        _opened(false)
      , _close_on_disconnect(true)
      , _type(type)
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

    ProcessorType Processor::type() const {
        return _type;
    }

    bool Processor::typeIs(ProcessorType value) const {
        return _type == value;
    }

    void Processor::setOpened(bool opened) {
        _opened = opened;
    }

    Code fpp::Processor::connectTo(fpp::Processor* other) {
        return_if(not_inited_ptr(other), Code::INVALID_INPUT);
        _next_processor_list.push_back(other);
        log_debug("Connected to " << other->name());
        return Code::OK;
    }

    Code Processor::disconnectFrom(Processor* other) {
        return_if(not_inited_ptr(other), Code::INVALID_INPUT);
        _next_processor_list.remove(other);
        log_debug("Disconnected from " << other->name());
        if (_next_processor_list.empty()) {
            if (_close_on_disconnect) {
                log_info("TODO самоуничтожение объекта"); //TODO заменить return type на void?
            } else {
                log_warning("Connection list is empty");
            }
        }
        return Code::OK;
    }

    void Processor::setCloseOnDisconnect(bool value) {
        _close_on_disconnect = value;
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
