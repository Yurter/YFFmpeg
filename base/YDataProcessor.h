#pragma once

#include "ffmpeg.h"
#include "utils.h"
#include "YThread.h"
#include "YAsyncQueue.h"
#include "base/YData.h"
#include "base/YPacket.h"

template <class inType, class outType>
class YDataProcessor : public YThread
{

public:

//    YDataProcessor() :
//        _next_processor(nullptr) {}

    virtual ~YDataProcessor()
    {
        _input_queue.clear();
    }

//    template <class nextInType, class nextOutType>
//    virtual void connectOutputTo(YDataProcessor<inType2,outType3>* next_processor) = 0;
//    {
//        _next_processor = next_processor;
//    }

    virtual void connectOutputTo(YThread* next_processor) = 0;

    virtual void push(inType input_data) final
    {
        _input_queue.push(input_data);
    }

protected:

    virtual YCode processInputData(inType& input_data) = 0;
    virtual YCode sendOutputData(outType output_data) final
    {
        if (_next_processor == nullptr) {
            return YCode::NOT_INITED;
        }
        _next_processor->push(output_data);
        return YCode::OK;
    }

private:

    YCode run() override final
    {
        inType input_data;
        if (!_input_queue.pop(input_data)) {
            utils::sleep_for(SHORT_DELAY_MS);
            return YCode::AGAIN;
        }
        return processInputData(input_data);
    }

private:

    YAsyncQueue<inType>     _input_queue;

protected:


    YThread*                _next_processor;

};
