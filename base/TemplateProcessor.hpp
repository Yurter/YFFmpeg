#pragma once
#include "core/utils.hpp"
#include "core/Thread.hpp"
#include "core/async/AsyncQueue.hpp"
#include "Processor.hpp"

namespace fpp {

    template <class inType, class outType>
    class TemplateProcessor : public Processor {

    public:

        using InputQueue = AsyncQueue<inType>;
        using OutputQueue = AsyncQueue<outType>;

        using IOFunction = std::function<Code(void)>;
        using ReadFunction = std::function<Code(inType&)>;
        using WriteFunction = std::function<Code(outType&)>;
        using ProcessFunction = std::function<Code(outType&)>;
        using DataVerification = std::function<Code(const inType&)>;

        TemplateProcessor() :
            _input_queue(50 MEGABYTES,  [](const inType& input_data)   { return input_data.size();  })
          , _output_queue(50 MEGABYTES, [](const outType& output_data) { return output_data.size(); }) {
            setName("TemplateProcessor");
        }

        virtual ~TemplateProcessor() {
            stopWait();
            if_not(_input_queue.empty()) {
                log_warning("Closed but input queue is not empty: " << _input_queue.length());
            }
            if_not(_output_queue.empty()) {
                log_warning("Closed but output queue is not empty: " << _output_queue.length());
            }
        }

        //TODO функции с буфером кривокод
        bool buferIsEmpty() {
            return _input_queue.empty();
        }

        int64_t bufferSize() {
            return _input_queue.size();
        }

        int64_t bufferLength() {
            return _input_queue.length();
        }

        virtual Code push(const Object* input_data) override final {
            increaseInputDataCount();
            if (closed()) {
                log_warning("Got " << input_data->name() << " but closed");
            }
            return_warning_if_not(
                _input_queue.wait_and_push(*static_cast<const inType*>(input_data))
                , "Failed to store " << input_data->name()
                , Code::EXIT
            );
            return Code::OK;
        }
//        Code push(const inType& input_data) {
//            increaseInputDataCount();
//            if (closed()) {
//                log_warning("Got " << input_data->name() << " but closed");
//            }
//            return_warning_if_not(
//                _input_queue.wait_and_push(input_data)
//                , "Failed to store " << input_data->name()
//                , Code::EXIT
//            );
//            return Code::OK;
//        }

    protected:

        virtual Code processInputData(inType input_data) = 0;
        virtual Code readInputData  ([[maybe_unused]] inType& input_data ) { return Code::NOT_IMPLEMENTED; }
        virtual Code writeOutputData([[maybe_unused]] outType output_data) { return Code::NOT_IMPLEMENTED; }

        Code setStartDataPred(const DataVerification&& pred) {
            return_if_not(pred, Code::ERR);
            _start_data_pred = pred;
            return Code::OK;
        }

        Code setPreFunction(const IOFunction pre_function) {
            return_if_not(pre_function, Code::ERR);
            _pre_function = pre_function;
            return Code::OK;
        }

        Code setPostFunction(const IOFunction post_function) {
            return_if_not(post_function, Code::ERR);
            _post_function = post_function;
            return Code::OK;
        }

        Code storeInputData(const inType& input_data) {
            return_if_not(_input_queue.push(input_data), Code::EXIT);
            return Code::OK;
        }

        Code storeOutputData(const outType& output_data) {
            return_if_not(_output_queue.push(output_data), Code::EXIT);
            return Code::OK;
        }

        Code restoreOutputData(outType& output_data) {
            return_if_not(_output_queue.wait_and_pop(output_data), Code::EXIT);
            return Code::OK;
        }

        Code sendOutputData(const outType& output_data, int64_t stream_uid = BROADCAST) {
            increaseOutputDataCount();
            if_not(_stream_map.empty()) {
                if (stream_uid == BROADCAST) { //TODO кривой код 13.01
                    for (auto it = _stream_map.begin(); it != _stream_map.end(); ++it)
                    {
                      it->second.for_each([&output_data,this](auto& next_processor) {
                          static_log_trace("Sender", "Sending data to " << next_processor->name());
                          try_throw_static(next_processor->push(&output_data));
                          if (output_data.typeIs(MediaType::EndOF)) { //Debug log
                              static_log_error("Debug", "Send EOF to " << next_processor->name());
                          }
                      });
                    }
                } else {
                    _stream_map[stream_uid].for_each([&output_data,this](auto& next_processor) {
                        static_log_trace("Sender", "Sending data to " << next_processor->name());
                        try_throw_static(next_processor->push(&output_data));
                    });
                }
            }
            return Code::OK;
        }

        void stopWait() {
            _input_queue.stop_wait();
            _output_queue.stop_wait();
        }

        Code sendEof() {
            log_debug("Sending EOF");
            outType eof;
            eof.setType(MediaType::EndOF);
            try_to(sendOutputData(eof));
            return Code::OK;
        }

    private:

        virtual Code run() override final {
            if /*constexpr*/ (_pre_function) {
                log_trace("Running _pre_function");
                try_to(_pre_function());
            }

            inType input_data;
            return_if_not(_input_queue.wait_and_pop(input_data), Code::EXIT);

            if (_start_data_pred) {
                if (Code ret = _start_data_pred(input_data); ret == Code::AGAIN) {
                    increaseDiscardedDataCount();
                    return ret;
                }
                _start_data_pred = nullptr;
            }

            if (input_data.isEOF()) {
                log_debug("Got EOF " << input_data.name());
                try_to(sendEof());
                return Code::END_OF_FILE;
            }

            log_trace("Running processInputData");
            try_to(processInputData(input_data));
            increaseProcessedDataCount();

            if /*constexpr*/ (_post_function) {
                log_trace("Running _post_function");
                try_to(_post_function());
            }

            return Code::OK;
        }

    private:

        DataVerification    _start_data_pred;

        IOFunction          _pre_function;
        IOFunction          _post_function;

        InputQueue          _input_queue;
        OutputQueue         _output_queue;

    };

} // namespace fpp
