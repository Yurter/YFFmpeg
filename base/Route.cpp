#include "Route.hpp"

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
        return Code::OK;
    }

    std::string Route::toString() const {
        return "TODO";
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


} // namespace fpp
