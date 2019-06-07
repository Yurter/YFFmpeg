#pragma once

#include "ffmpeg.h"
#include "utils.h"
#include "YThread.h"
#include "YAsyncQueue.h"

template <class inType, class outType>
class YDataProcessor /*: public YThread*/
{

public:

    YDataProcessor() :
        _next_processor(nullptr) {}

    virtual ~YDataProcessor()
    {
        _input_queue.clear();
        _output_queue.clear();
    }

    void connectOutputTo(YDataProcessor* next_processor)
    {
        _next_processor = next_processor;
    }

protected:

    virtual YCode processInputData(inType input_data) = 0;

    void push(inType input_data)
    {
        _input_queue.push(input_data);
    }

    bool pop(outType &output_data)
    {
        return _output_queue.pop(output_data);
    }

    YCode run() //TODO
    {
        inType input_data;
        if (!_input_queue.pop(input_data)) {
            utils::sleep_for(SHORT_DELAY_MS);
            return YCode::AGAIN;
        }
        auto ret = processInputData(input_data);
        if (ret != YCode::OK) { return ret; }
    }

protected:

    YAsyncQueue<inType>     _input_queue;
    YAsyncQueue<outType>    _output_queue;

private:

    YDataProcessor*         _next_processor;

    YThread                 _input_thread;
    YThread                 _output_thread;

};
