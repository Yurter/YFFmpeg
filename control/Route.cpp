#include "Route.hpp"
#include "core/utils.hpp"

namespace fpp {

    Route::Route()
        : _input_stream_uid(INVALID_INT)
        , _output_stream_uid(INVALID_INT){
        setName("Route");
    }

    Route::~Route() { //TODO
//        log_error("DESTR");
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
        _sequence.for_each([uid = _input_stream_uid](ProcessorPointer& current, ProcessorPointer& next) {
            try_throw_static(current->connectTo(uid, next));
        });
        setInited(true);
        return Code::OK;
    }

    std::string Route::toString() const {
        if (_sequence.empty()) {
            return "empty";
        }
        std::string str;
        const std::string delimeter = " -> ";
        _sequence.for_each([&str,&delimeter](const auto& item) {
            str += std::to_string(item->uid()) + "_";
            str += item->name();
            str += delimeter;
        });
        str.erase(str.size() - delimeter.size(), delimeter.size());
        return str;
    }

    Code Route::append(ProcessorPointer processor) {
        return_if(not_inited_ptr(processor), Code::INVALID_INPUT);
        _sequence.push_back(processor);
        return Code::OK;
    }

    Code Route::setMetaRoute(int64_t input_stream_uid, int64_t output_stream_uid) {
        return_if(input_stream_uid == output_stream_uid, Code::INVALID_INPUT);
        return_if(not_inited_int(input_stream_uid),  Code::INVALID_INPUT);
        return_if(not_inited_int(output_stream_uid), Code::INVALID_INPUT);
        _input_stream_uid = input_stream_uid;
        _output_stream_uid = output_stream_uid;
        return Code::OK;
    }

    int64_t Route::inputStreamUid() const {
        return _input_stream_uid;
    }

    int64_t Route::outputStreamUid() const {
        return _output_stream_uid;
    }

    bool Route::contains(const int64_t uid) const {
        bool exist = false;
        _sequence.for_each([uid,&exist](const auto& item) {
            if (item->uid() == uid) {
                exist = true;
            }
        });
        return exist;
    }

    Code Route::startAll() { //TODO кривой код
        _sequence.for_each([this](auto& item) {
//            try_to(item->init());
//            try_to(item->open());
//            try_to(item->start());
            item->init();
            item->open();
            item->start();
        });
        return Code::OK;
    }

    Code Route::destroy() { //TODO
        _sequence.access([](auto seq){
            for (auto processor = seq.begin(); processor != seq.end(); processor++) {
                auto next_processor = std::next(processor);
                return_if(next_processor == seq.end(), Code::OK);

                static_log_error("debug", "Dis " << (*processor)->name() << " from " << (*next_processor)->name());
                (*processor)->disconnectFrom(*next_processor);

//                return Code::OK;
//                return Code::NOT_IMPLEMENTED;
    //            проверка на оставшийся коннект после дисконекта
    //                    если конект лист не пуст -> break
            }
        });
//        for (auto processor = _sequence.begin(); processor != _sequence.end(); processor++) {
//            auto next_processor = std::next(processor);
//            return_if(next_processor == _sequence.end(), Code::OK);

//            try_throw((*processor)->get()->disconnectFrom((*next_processor)->get()));

//            return Code::NOT_IMPLEMENTED;
////            проверка на оставшийся коннект после дисконекта
////                    если конект лист не пуст -> break
//        }
        return Code::OK;
    }

    ProcessorSequence Route::processorSequence() {
        return _sequence;
    }

    Code Route::changePartTo(ProcessorVector other) { //TODO
        log_warning("From: " << this->toString());
        for (size_t i = 0; i < other.size(); ++i) {
            return_if(not_inited_ptr(other[i]), Code::INVALID_INPUT);
            try_to(_sequence[i]->disconnectFrom(_sequence[i + 1]));
            _sequence[i] = other[i];
        }
        auto debug_value_0 = other[other.size() - 1];
        auto debug_value_1 = _sequence[other.size()];
        try_to(other[other.size() - 1]->connectTo(_input_stream_uid, _sequence[other.size()]));
        log_warning("To: " << this->toString());
        return Code::OK;
    }

} // namespace fpp
