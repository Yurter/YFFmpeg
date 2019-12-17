#pragma once
#include "core/utils.hpp"
#include "core/Thread.hpp"
#include "core/AsyncDiscardSizeQueue.hpp"
#include "core/AsyncDiscardAmountQueue.hpp"
#include "Processor.hpp"

namespace fpp {

    template <class inType, class outType>
    class TemplateProcessor : public Processor {

    public:

        using IOFunction = std::function<Code(void)>;
        using WriteFunction = std::function<Code(outType&)>;
        using ProcessFunction = std::function<Code(outType&)>;
        using InputQueue = AsyncDiscardSizeQueue<inType>;
        using OutputQueue = AsyncDiscardSizeQueue<outType>;

        TemplateProcessor() {
            setName("TemplateProcessor");
        }

        virtual ~TemplateProcessor() {
            stopWait();
        }

        bool buferIsEmpty() {
            return _input_queue.empty();
        }

        int64_t bufferSize() {
            return _input_queue.size();
        }

        virtual Code push(const Object* input_data) override final {
            if (closed()) {
                log_warning("Got " << input_data->name() << " but closed");
            }
            if (!_input_queue.push(*static_cast<const inType*>(input_data))) {
                log_warning("Failed to store " << input_data->name());
            }
            return Code::OK;
        }

    protected:

        virtual Code processInputData(inType input_data) = 0;
        virtual Code readInputData(inType& input_data) { UNUSED(input_data); return Code::NOT_IMPLEMENTED; }
        virtual Code writeOutputData(outType output_data) { UNUSED(output_data); return Code::NOT_IMPLEMENTED; }

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

        Code sendOutputData(const outType& output_data) {
            std::lock_guard lock(_proc_mutex);
//            if (this->is("Encoder")) {
//                static_log_error("TODO", "Sending data to " << _next_processor_list.size());
//            }
            _next_processor_list.for_each([&output_data,this](auto& next_processor) {
                static_log_trace("TODO", "Sending data to " << next_processor->name());
                try_throw_static(next_processor->push(&output_data));
            });
            return Code::OK;
        }

        void stopWait() {
            _input_queue.clear();
            _output_queue.clear();
            _input_queue.stop_wait();
            _output_queue.stop_wait();
        }

    private:

        virtual Code run() override final {
            log_trace("Running run.");

            if (_pre_function) {
                log_trace("Running _pre_function.");
//                try_to(_pre_function());
                Code ret = _pre_function();
                if (utils::exit_code(ret)
                        && (ret != Code::END_OF_FILE)) {
                    return ret;
                }
                return_if(ret == Code::AGAIN, ret); //TODO возожно следует занести AGAIN в еррор код
            }

            inType input_data;
            return_if_not(_input_queue.wait_and_pop(input_data), Code::EXIT);

            /* Не удалять проверки! */
//            return_if(discardType(input_data.type()), Code::AGAIN);
//            log_warning(utils::bool_to_string(input_data.empty()));
//            return_error_if(input_data.typeIs(MediaType::MEDIA_TYPE_UNKNOWN)
//                            , "Got " << utils::media_type_to_string(MediaType::MEDIA_TYPE_UNKNOWN) << " data."
//                            , Code::INVALID_INPUT);

//////////////////////////////////////////////////////////////////
            //TODO
//            if (input_data.empty()
//                    && this->is("MediaSink")) {
//                return Code::END_OF_FILE;
//            }
            if (input_data.empty()
                    && !this->is("MediaSource") // вся разница в этом условии
                    && !this->is("CustomPacketSource")) {
                sendOutputData(outType());
                return Code::END_OF_FILE;
            }
            /* TODO не работает это условие :( */
//            if (input_data.empty()) {
//                try_to(sendOutputData(outType()));
//                return Code::END_OF_FILE;
//            }
//////////////////////////////////////////////////////////////////

            log_trace("Running processInputData.");
            try_to(processInputData(input_data));

            if (_post_function) {
                log_trace("Running _post_function.");
                try_to(_post_function());
            }

            return Code::OK;
        }

    private:

        IOFunction          _pre_function;
        IOFunction          _post_function;

        InputQueue          _input_queue;
        OutputQueue         _output_queue;

    };

} // namespace fpp
