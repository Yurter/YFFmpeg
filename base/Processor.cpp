#include "Processor.hpp"
#include "core/utils.hpp"

namespace fpp {

    Processor::Processor()
        : _uid { utils::gen_uid() }
        , _type { ProcessorType::Unknown }
        , _opened { false }
        , _input_data_count { 0 }
        , _output_data_count { 0 }
        , _discarded_data_count { 0 }
        , _processed_data_count { 0 } {
        setName("Processor");
    }

    Processor::~Processor() {
        log_debug("Data usage brief: "
                  << "in " << inputDataCount() << ", "
                  << "discarded " << discardedDataCount() << ", "
                  << "processed " << processedDataCount() << ", "
                  << "out " << outputDataCount()
        );
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

    StreamVector Processor::streams() const {
        return _streams;
    }

    StreamPointer Processor::bestStream(MediaType type) const {
        StreamVector candidate_list;
        for (auto& candidate : streams()) {
            if (candidate->typeIs(type)) { candidate_list.push_back(candidate); }
        }
        auto ret = std::max_element(std::begin(candidate_list), std::end(candidate_list), [](const StreamPointer& lhs, const StreamPointer& rhs) {
            return lhs->params->betterThen(rhs->params);
        });
        return_if(ret == std::end(candidate_list), nullptr);
        return *ret;
    }

    StreamPointer Processor::stream(int64_t index) const {
        if (size_t(index) < _streams.size()) {
            return _streams[size_t(index)];
        } else {
            return nullptr;
        }
    }

    Code Processor::setStreams(StreamVector streams) {
        return_if_not(_streams.empty(), Code::INVALID_CALL_ORDER); //TODO нужна ли эта проверка? 15.01
        _streams = streams;
        std::for_each(_streams.begin(), _streams.end()
            , [this_context_uid = uid()](auto& stream) {
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

    Code fpp::Processor::connectTo(StreamId_t stream_id, ProcessorPointer other) {
        return_if(not_inited_ptr(other), Code::INVALID_INPUT);
        _stream_map[stream_id].push_back(other);
        log_debug("Connected " << stream_id << " stream to " << other->name());
        return Code::OK;
    }

    Code Processor::disconnectFrom(const ProcessorPointer other) { //TODO нет проверок на ошибки 10.01
        return_if(not_inited_ptr(other), Code::INVALID_INPUT);
        for (auto [stream_id, proc_list] : _stream_map) {
            proc_list.remove_if([&other](const auto& proc) {
                return proc->uid() == other->uid();
            });
        }
        log_debug("Disconnected from " << other->name());
        return Code::OK;
    }

    bool Processor::equalTo([[maybe_unused]] const ProcessorPointer other) const {
        log_warning("Method equalTo() was not implemented");
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

    uint64_t Processor::inputDataCount() const {
        return _input_data_count;
    }

    uint64_t Processor::outputDataCount() const {
        return _output_data_count;
    }

    uint64_t Processor::discardedDataCount() const {
        return _discarded_data_count;
    }

    uint64_t Processor::processedDataCount() const {
        return _processed_data_count;
    }

    void Processor::increaseInputDataCount() {
        _input_data_count++;
    }

    void Processor::increaseOutputDataCount() {
        _output_data_count++;
    }

    void Processor::increaseDiscardedDataCount() {
        _discarded_data_count++;
    }

    void Processor::increaseProcessedDataCount() {
        _processed_data_count++;
    }

} // namespace fpp
