#pragma once

#include "ffmpeg.h"
#include "utils.h"
#include "YThread.h"
#include "YAsyncQueue.h"

template <class inType, class outType>
class YDataProcessor
{

public:

    YDataProcessor() :
        _next_processor(nullptr) {}

    virtual ~YDataProcessor()
    {
        _input_thread.quit();
        _output_thread.quit();
        _input_queue.clear();
    }

    void connectOutputTo(YDataProcessor* next_processor)
    {
        _next_processor = next_processor;
    }

    void push(inType input_data)
    {
        _input_queue.push(input_data);
    }

    YCode start()
    {
        if (_next_processor == nullptr) {
            return YCode::NOT_INITED;
        }

        _input_thread = [this](){
            inType input_data;
            if (!_input_queue.pop(input_data)) {
                utils::sleep_for(SHORT_DELAY_MS);
                return YCode::AGAIN;
            }
            auto ret = processInputData(input_data)
        };

        _output_thread = [this](){
            //
        };

        _input_thread.start();
        _output_thread.start();

        if (!_input_thread.running() || !_output_thread.running()) {
            _input_thread.quit();
            _output_thread.quit();
            return YCode::ERR;
        }
        return YCode::OK;
    }

protected:

    virtual YCode processInputData(inType& input_data, YDataProcessor* next_processor) = 0;

private:

    YCode run() override final
    {
        if (_next_processor == nullptr) {
            return YCode::NOT_INITED;
        }
        inType input_data;
        if (!_input_queue.pop(input_data)) {
            utils::sleep_for(SHORT_DELAY_MS);
            return YCode::AGAIN;
        }
        outType output_data;
        auto ret = processInputData(input_data, output_data);
        if (!utils::exit_code(ret)) {
            _next_processor->push(input_data);
        }
        return ret;
    }

private:

    YThread                 _input_thread;
    YThread                 _output_thread;
    YAsyncQueue<inType>     _input_queue;
    YAsyncQueue<outType>    _output_queue;
    YDataProcessor*         _next_processor;

};
