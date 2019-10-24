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
            stopInOutThread();
        }

        bool buferIsEmpty() const {
            return _input_queue.empty();
        }

    protected:

        virtual Code processInputData(inType input_data) = 0;
        virtual Code readInputData(inType& input_data) { UNUSED(input_data); return Code::NOT_IMPLEMENTED; }
        virtual Code writeOutputData(outType output_data) { UNUSED(output_data);  return Code::NOT_IMPLEMENTED; }

        Code setInOutFunction(const IOFunction io_function) {
            return_if(_io_function, Code::ERR);
            _io_function = io_function;
        }

        Code sendOutputData(const outType& output_data, const Processor* next_proc = nullptr) {
            auto pointer = inited_ptr(next_proc) ? next_proc : _next_processor;
            return_if(not_inited_ptr(pointer), Code::ERR);
            return_if_not(pointer->push(output_data), Code::EXIT);
            return Code::OK;
        }

    private:

        virtual Code onStart() override final {
            try_to(startInOutThread());
        }

        virtual Code run() override final {
            inType input_data;
            return_if_not(wait_and_pop(input_data), Code::EXIT);
            return_if(ignoreType(input_data.type()), Code::AGAIN);
            if (input_data.empty() && !this->is("YMap") && !this->is("Source")) { //Source тоже?
                sendOutputData(outType());
                return Code::END_OF_FILE;
            }
            return processInputData(input_data);
        }

        Code startInOutThread() {
            _io_thread = Thread(_io_function);
            _io_thread.start();
            _io_thread.setName(name() + " IO thread");
        }

        Code stopInOutThread() {
            _io_thread.quit();
            _io_thread.join();
        }

    private:

        AsyncQueue<inType>  _input_queue;
        IOFunction          _io_function;
        Thread              _io_thread;

    };

} // namespace fpp
