#include "Route.hpp"
#include "core/utils.hpp"

namespace fpp {

    Route::Route() {
        setName("Route");
    }

    Route::~Route() {
        for (auto processor : _sequence) {
            //TODO последовательное отключение
        }
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

    Code Route::startAll() {
        for (auto&& elem : _sequence) {
            log_error("OPENING: " << elem->name());
            try_to(elem->open());
            try_to(elem->start());
        }
        return Code::OK;
    }


} // namespace fpp
