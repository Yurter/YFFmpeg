#pragma once

#include "ffmpeg.h"
#include "utils.h"
#include "YThread.h"
#include "YAsyncQueue.h"

template <class inType, class outType>
class YDataProcessor : YThread
{

public:

    YDataProcessor() :
        _next_processor(nullptr) {}

    virtual ~YDataProcessor()
    {
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

protected:

    virtual YCode processInputData(inType& input_data, inType& output_data) = 0;

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

    YAsyncQueue<inType>     _input_queue;
    YDataProcessor*         _next_processor;

};
