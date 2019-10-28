#pragma once
#include "core/utils.hpp"
#include "core/Thread.hpp"
#include "core/AsyncQueue.hpp"
#include "Processor.hpp"

namespace fpp {

    template <class inType, class outType>
    class TemplateProcessor : public Processor {

    public:

        using IOFunction = std::function<Code()>;

        TemplateProcessor() {
            setName("TemplateProcessor");
        }

        virtual ~TemplateProcessor() {
            //stopInOutThread();
        }

        bool buferIsEmpty() const {
            return _input_queue.empty();
        }

        virtual Code push(const Object* input_data) override final {
            return_if_not(_input_queue.wait_and_push(*static_cast<const inType*>(input_data)), Code::EXIT);
            return Code::OK;
        }

    protected:

        virtual Code processInputData(inType input_data) = 0;
        virtual Code readInputData(inType& input_data) { UNUSED(input_data); return Code::NOT_IMPLEMENTED; }
        virtual Code writeOutputData(outType output_data) { UNUSED(output_data);  return Code::NOT_IMPLEMENTED; }

        Code setPreFunction(const IOFunction pre_function) {
            return_if(pre_function, Code::ERR);
            _pre_function = pre_function;
        }

        Code setPostFunction(const IOFunction post_function) {
            return_if(post_function, Code::ERR);
            _post_function = post_function;
        }

//        Code setInOutFunction(const IOFunction io_function) {
//            return_if(_io_function, Code::ERR);
//            _io_function = io_function;
//        }

        Code sendOutputData(const outType& output_data, Processor* next_proc = nullptr) {
            auto pointer = inited_ptr(next_proc) ? next_proc : _next_processor;
            return_if(not_inited_ptr(pointer), Code::ERR);
            return_if_not(pointer->push(&output_data), Code::EXIT);
            return Code::OK;
        }

        Code storeOutputData(const outType& output_data) {
            return_if_not(_post_queue.wait_and_push(output_data), Code::EXIT);
            return Code::OK;
        }

        Code restoreOutputData(outType& output_data) {
            return_if_not(_post_queue.wait_and_pop(output_data), Code::EXIT);
            return Code::OK;
        }

    private:

//        virtual Code onStart() override final {
//            try_to(startInOutThread());
//        }

        virtual Code run() override final {
            if (_pre_function) { try_to(_pre_function()); }
            inType input_data;
            return_if_not(_input_queue.wait_and_pop(input_data), Code::EXIT);
            return_if(discardType(input_data.type()), Code::AGAIN);
            if (input_data.empty() && !this->is("YMap") && !this->is("Source")) { //Source тоже?
                sendOutputData(outType());
                return Code::END_OF_FILE;
            }
            try_to(processInputData(input_data));
            if (_post_function) { try_to(_post_function()); }
            return Code::OK;
        }

//        Code startInOutThread() {
//            _io_thread = Thread(_io_function);
//            _io_thread.start();
//            _io_thread.setName(name() + " IO thread");
//        }

//        Code stopInOutThread() {
//            _io_thread.quit();
//            _io_thread.join();
//        }

    private:

        AsyncQueue<inType>  _input_queue;
//        IOFunction          _io_function;
//        Thread              _io_thread;

        IOFunction          _pre_function;
        IOFunction          _post_function;

        AsyncQueue<outType> _post_queue;

    };

} // namespace fpp
