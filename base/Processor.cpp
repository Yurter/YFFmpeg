#include "Processor.hpp"
#include "core/utils.hpp"

namespace fpp {

    Processor::Processor() :
        _uid(utils::gen_uid())
      , _type(ProcessorType::Unknown)
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

    const StreamVector &Processor::streams() const {
        return _streams;
    }

    Stream* Processor::stream(int64_t index) const {
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

    Code Processor::setStreams(StreamVector streams) {
        return_if(streams.empty(), Code::INVALID_INPUT);
        return_if_not(_streams.empty(), Code::INVALID_CALL_ORDER);
        _streams = streams;
        const int64_t this_context_uid = uid();
        std::for_each(_streams.begin(), _streams.end()
            ,[this_context_uid](Stream*& stream) {
            stream->params->setContextUid(this_context_uid);
        });
        return Code::OK;
    }

    void Processor::setType(ProcessorType value) {
        _type = value;
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

    Code fpp::Processor::connectTo(const ProcessorPointer other) {
        std::lock_guard lock(_proc_mutex);
        return_if(not_inited_ptr(other), Code::INVALID_INPUT);
//        if (this->is("Encoder")) {
//            static_log_error("TODO", "0000000000 " << _next_processor_list.size());
//        }
        _next_processor_list.push_back(other);
//        if (this->is("Encoder")) {
//            static_log_error("TODO", "1111111111 " << _next_processor_list.size());
//        }
        log_debug("Connected to " << other->name());
//        if (running()) {
//            log_error("running = " << _next_processor_list.size() << " " << other->uid());
//        }
        return Code::OK;
    }

    Code Processor::disconnectFrom(const ProcessorPointer other) {
        std::lock_guard lock(_proc_mutex);
        return_if(not_inited_ptr(other), Code::INVALID_INPUT);
        if (this->is("MediaSource")) {
            static_log_error("TODO", "dis_0 " << _next_processor_list.size());
            _next_processor_list.for_each([other](const auto& proc) {
                static_log_error("TODO", "UID -> " << proc->uid() << ", other " << other->uid());
            });
        }
        _next_processor_list.remove_if([&other](const auto& sink) {
            return sink->uid() == other->uid();
        });
        if (this->is("MediaSource")) {
            static_log_error("TODO", "dis_1 " << _next_processor_list.size());
        }
        log_debug("Disconnected from " << other->name());
        if (_next_processor_list.empty()) {
            if (_close_on_disconnect) {
                log_info("TODO self destroy"); //TODO заменить return type на void?
                try_to(stop());
            } else {
                log_warning("Connection list is empty");
            }
        }
        return Code::OK;
    }

    bool Processor::equalTo(const ProcessorPointer other) const {
        UNUSED(other);
        log_warning("Method equalTo() was not implemented.");
        return false;
    }

    ProcessorType Processor::type() const {
        return _type;
    }

    bool Processor::typeIs(ProcessorType value) const {
        return _type == value;
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
