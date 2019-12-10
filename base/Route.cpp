#include "Route.hpp"
#include "core/utils.hpp"

namespace fpp {

    Route::Route() :
        _source(nullptr)
      , _sink(nullptr)
    {
        setName("Route");
    }

    Route::~Route() { //TODO
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
        return_if(not_inited_ptr(_source), Code::INVALID_INPUT);
        return_if(not_inited_ptr(_sink), Code::INVALID_INPUT);
//        if (_sequence.size() < 2) {
//            return Code::INVALID_INPUT;
//        }
        if (inited()) {
            return Code::INVALID_CALL_ORDER;
        }
        for (auto processor = _sequence.begin(); processor != _sequence.end(); processor++) {
            auto next_processor = std::next(processor);
            if (next_processor == _sequence.end()) {
                return Code::OK;
            }
            try_to((*processor)->get()->connectTo((*next_processor)->get()));
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
            str += std::to_string(elem->get()->uid()) + "_";
            str += elem->get()->name();
            str += delimeter;
        }
        str.erase(str.size() - delimeter.size(), delimeter.size());
        return str;
    }

    Code Route::setSource(ProcessorPtr* source) {
        return_if(not_inited_ptr(source), Code::INVALID_INPUT);
        _source = source;
    }

    Code Route::setSink(ProcessorPtr* sink) {
        return_if(not_inited_ptr(sink), Code::INVALID_INPUT);
        _sink = sink;
    }

    Code Route::append(ProcessorPtr processor) {
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

    bool Route::contains(const int64_t uid) const {
        bool exist = false;
        std::for_each(_sequence.begin(), _sequence.end()
            , [uid,&exist](const auto& elem) {
                if (elem->get()->uid() == uid) {
                    exist = true;
                }
            }
        );
        return exist;
    }

    Code Route::startAll() {
        for (auto&& elem : _sequence) {
            try_to(elem->get()->init());
            try_to(elem->get()->open());
            try_to(elem->get()->start());
        }
        return Code::OK;
    }

    Code Route::destroy() {
        for (auto processor = _sequence.begin(); processor != _sequence.end(); processor++) {
            auto next_processor = std::next(processor);
            return_if(next_processor == _sequence.end(), Code::OK);

            try_throw((*processor)->get()->disconnectFrom((*next_processor)->get()));

            return Code::NOT_IMPLEMENTED;
//            проверка на оставшийся коннект после дисконекта
//                    если конект лист не пуст -> break
        }
        return Code::OK;
    }

    ProcessorSequence Route::processorSequence() {
        return _sequence;
    }

    Code Route::changePartTo(ProcessorVector other) {
        log_warning("From: " << this->toString());
        //for (auto& proc : other) {
        for (size_t i = 0; i < other.size(); ++i) {
            return_if(not_inited_ptr(other[i]), Code::INVALID_INPUT);
            try_to(_sequence[i]->get()->disconnectFrom(_sequence[i + 1]->get()));
            _sequence[i] = &other[i];
        }
        try_to(other[other.size() - 1]->connectTo(_sequence[other.size() - 1]->get()));
        log_warning("To: " << this->toString());
        return Code::OK;
    }

} // namespace fpp
