#pragma once

#include "ffmpeg.h"
#include "YThread.h"
#include "YAsyncQueue.h"

template <class inType, class outType>
class YDataProcessor : public YThread
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

    void push(inType input_data)
    {
        _input_queue.push(input_data);
    }

    bool pop(outType &output_data)
    {
        return _output_queue.pop(output_data);
    }

protected:

    YAsyncQueue<inType>     _input_queue;
    YAsyncQueue<outType>    _output_queue;

private:

    YDataProcessor*         _next_processor;

};
