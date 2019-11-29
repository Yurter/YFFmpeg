#include "Processor.hpp"
#include "core/utils.hpp"

namespace fpp {

    Processor::Processor() :
        _uid(utils::gen_uid())
      , _opened(false)
      , _close_on_disconnect(true)
      , _discard_types(MediaType::MEDIA_TYPE_UNKNOWN)
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


    int64_t Processor::uid() const {
        return _uid;
    }

    StreamVector Processor::streams() {
        return _streams;
    }

    Stream* Processor::stream(int64_t index) {
        if (size_t(index) < _streams.size()) {
            return _streams[size_t(index)];
        } else {
            return nullptr;
        }
    }

    Code Processor::addStream(Stream* stream) {
        return_if(not_inited_ptr(stream), Code::INVALID_INPUT);
        _streams.push_back(stream);
        log_info("Stream added: " << stream->toString());
        return Code::OK;
    }

    void Processor::setOpened(bool opened) {
        _opened = opened;
    }

    void Processor::setCloseOnDisconnect(bool value) {
        _close_on_disconnect = value;
    }


    void Processor::setDiscardType(MediaType type) {
        _discard_types |= type;
    }

    void Processor::setMetaData(const std::string& value) {
        _meta_data = value;
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

    bool Processor::opened() const {
        return _opened;
    }

    bool Processor::closed() const {
        return !_opened;
    }

    bool Processor::discardType(MediaType type) const {
        return _discard_types & type;
    }

    std::string Processor::metaData() const {
        return _meta_data;
    }

} // namespace fpp
