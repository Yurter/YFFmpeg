#pragma once
#include "utils.hpp"
#include "Thread.hpp"
#include "AsyncQueue.hpp"
#include "Processor.hpp"

namespace fpp {

    template <class inType, class outType>
    class TemplateProcessor : public Processor {

    public:

        using InputFunction = std::function<inType()>;
        using OutputFunction = std::function<outType()>;

        TemplateProcessor()
        {
            setName("TemplateProcessor");
        }

        virtual ~TemplateProcessor() = default;

    protected:

        virtual Code processInputData(const inType& input_data) = 0;

        Code setInputFunction(const InputFunction input_function) {
            return_if(_input_function, Code::ERR);
            return_if(_output_function, Code::ERR);
            _input_function = input_function;
        }

        Code setOutputFunction(const OutputFunction output_function) {
            return_if(_input_function, Code::ERR);
            return_if(_output_function, Code::ERR);
            _output_function = output_function;
        }

        Code sendOutputData(const outType& output_data, const Processor* next_proc = nullptr) {
            auto pointer = inited_ptr(next_proc) ? next_proc : _next_processor;
            return_if(not_inited_ptr(pointer), Code::ERR);
            return_if_not(pointer->push(output_data), Code::EXIT);
            return Code::OK;
        }

    private:

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

    private:

        AsyncQueue<inType>  _input_queue;

        InputFunction       _input_function;
        OutputFunction      _output_function;
        Thread              _io_thread;

    };

} // namespace fpp
