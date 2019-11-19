#pragma once
#include "core/utils.hpp"
#include "core/Thread.hpp"
#include "core/AsyncDiscardQueue.hpp"
#include "Processor.hpp"

namespace fpp {

    template <class inType, class outType>
    class TemplateProcessor : public Processor {

    public:

        using IOFunction = std::function<Code()>;

        TemplateProcessor(ProcessorType type) :
            Processor(type)
        {
            setName("TemplateProcessor");
        }

        virtual ~TemplateProcessor() {
            //TODO
        }

        bool buferIsEmpty() {
            return _input_queue.empty();
        }

        int64_t bufferSize() {
            return _input_queue.size();
        }

        virtual Code push(const Object* input_data) override final {
            return_if(closed(), Code::AGAIN);
            if (!_input_queue.push(*static_cast<const inType*>(input_data))) {
                log_warning("queue push failed");
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
        }

        Code setPostFunction(const IOFunction post_function) {
            return_if_not(post_function, Code::ERR);
            _post_function = post_function;
        }

        Code sendOutputData(const outType& output_data) {
            for (auto next_processor : _next_processor_list) {
                try_to(next_processor->push(&output_data));
            }
            return Code::OK;
        }

        Code storeInputData(const inType& input_data) {
            return_if_not(_input_queue.push(input_data), Code::EXIT);
            return Code::OK;
        }

        Code storeOutputData(const outType& output_data) {
            return_if_not(_post_queue.push(output_data), Code::EXIT);
            return Code::OK;
        }

        Code restoreOutputData(outType& output_data) {
            return_if_not(_post_queue.wait_and_pop(output_data), Code::EXIT);
            return Code::OK;
        }

    private:

        virtual Code run() override final {
            if (_pre_function) { try_to(_pre_function()); }
            inType input_data;
            return_if_not(_input_queue.wait_and_pop(input_data), Code::EXIT);

            return_if((input_data.empty() == false)
                      && discardType(input_data.type()), Code::AGAIN);

            if (input_data.empty()) {
                log_error("Got emty data!");
                if (this->is("MediaSink")) {
                    int tt = 9;
                    tt += 1;
                }
            }
            //TODO
            if (input_data.empty()
                    && this->is("MediaSink")) {
                return Code::END_OF_FILE;
            }
            if (input_data.empty()
                    && !this->is("YMap")
                    && !this->is("MediaSource")
                    && !this->is("CustomPacketSource")) {
                sendOutputData(outType());
                return Code::END_OF_FILE;
            }
            try_to(processInputData(input_data));
            if (_post_function) { try_to(_post_function()); }
            return Code::OK;
        }

    private:

        AsyncDiscardQueue<inType>  _input_queue;

        IOFunction          _pre_function;
        IOFunction          _post_function;

        AsyncDiscardQueue<outType> _post_queue;

    };

} // namespace fpp
