#include "Route.hpp"
#include "core/utils.hpp"

namespace fpp {

    Route::Route() {
        setName("Route");
    }

//    Route::Route(const Route& other) {
//        setName("Route");
//        _input_stream_uid = other._input_stream_uid;
//        _output_stream_uid = other._output_stream_uid;
//        _sequence = other._sequence;
//    }

//    Route::Route(const Route&& other) {
//        setName("Route");
//        _input_stream_uid = other._input_stream_uid;
//        _output_stream_uid = other._output_stream_uid;
//        _sequence = std::move(other._sequence);
//    }

    Route::~Route() {
//        log_warning("deleting...");
//        for (auto processor = _sequence.begin(); processor != _sequence.end(); processor++) {
//            auto next_processor = std::next(processor);
//            if (next_processor == _sequence.end()) {
//                return;
//            }
//            try_throw((*processor)->disconnectFrom(*next_processor));
//        }
    }

    Code Route::init() {
        if (_sequence.size() < 2) {
            return Code::INVALID_INPUT;
        }
        if (inited()) {
            return Code::INVALID_CALL_ORDER;
        }
        for (auto processor = _sequence.begin(); processor != _sequence.end(); processor++) {
            auto next_processor = std::next(processor);
            if (next_processor == _sequence.end()) {
                return Code::OK;
            }
            try_to((*processor)->connectTo(*next_processor));
        }
        setInited(true);
        return Code::OK;
    }

    std::string Route::toString() const {
        if (_sequence.empty()) {
            return "empty";
        }
        std::string str;
        std::string delimeter = " -> ";
        for (auto&& elem : _sequence) {
            str += elem->name();
            str += delimeter;
        }
        str.erase(str.size() - delimeter.size(), delimeter.size());
        return str;
    }

    Code Route::setMetaRoute(int64_t input_stream_uid, int64_t output_stream_uid) {
        return_if(input_stream_uid == output_stream_uid, Code::INVALID_INPUT);
        return_if(not_inited_int(input_stream_uid),  Code::INVALID_INPUT);
        return_if(not_inited_int(output_stream_uid), Code::INVALID_INPUT);
        _input_stream_uid = input_stream_uid;
        _output_stream_uid = output_stream_uid;
        return Code::OK;
    }

    Code Route::append(Processor* processor) {
        return_if(not_inited_ptr(processor), Code::INVALID_INPUT);
        _sequence.push_back(processor);
        return Code::OK;
    }

    int64_t Route::inputStreamUid() const {
        return _input_stream_uid;
    }

    int64_t Route::outputStreamUid() const {
        return _output_stream_uid;
    }

    bool Route::contains(const Processor * const processor) const {
        for (auto&& elem : _sequence) {
            return_if(elem == processor, true);
        }
        return false;
    }

    Code Route::startAll() {
        for (auto&& elem : _sequence) {
//            log_error("OPENING: " << elem->name());
            try_to(elem->init());
            try_to(elem->open());
            try_to(elem->start());
            //
//            elem->init();
//            elem->open();
//            elem->start();
        }
        return Code::OK;
    }

    Code Route::destroy() {
        for (auto processor = _sequence.begin(); processor != _sequence.end(); processor++) {
            auto next_processor = std::next(processor);
            if (next_processor == _sequence.end()) {
                return Code::OK;
            }
            try_throw((*processor)->disconnectFrom(*next_processor));
        }
        return Code::OK;
    }

    ProcessorSequence Route::processorSequence() {
        return _sequence;
    }

    Code Route::changePartTo(ProcessorVector other) {
        for (auto& proc : other) {
            return_if(not_inited_ptr(0), Code::INVALID_INPUT);
        }
    }


} // namespace fpp
