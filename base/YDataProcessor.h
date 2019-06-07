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

    virtual YCode start() final
    {
        if (_next_processor == nullptr) {
            return YCode::NOT_INITED;
        }

        _input_thread = YThread([this](){
            inType input_data;
            if (!_input_queue.pop(input_data)) {
                utils::sleep_for(SHORT_DELAY_MS);
                return YCode::AGAIN;
            }
            return processInputData(input_data);
        });

        _output_thread = YThread([this](){
            outType output_data;
            if (!_output_queue.pop(output_data)) {
                utils::sleep_for(SHORT_DELAY_MS);
                return YCode::AGAIN;
            }
            _next_processor->push(output_data);
            return YCode::OK;
        });

        _input_thread.start();
        _output_thread.start();

        if (_input_thread.running() && _output_thread.running()) {
            return YCode::OK;
        } else {
            return YCode::ERR;
        }
    }

protected:

    YAsyncQueue<inType>     _input_queue;
    YAsyncQueue<outType>    _output_queue;

private:

    YDataProcessor*         _next_processor;

    YThread                 _input_thread;
    YThread                 _output_thread;

};
