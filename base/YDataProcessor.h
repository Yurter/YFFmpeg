#pragma once

#include "ffmpeg.h"
#include "utils.h"
#include "YThread.h"
#include "YAsyncQueue.h"
#include "base/YData.h"
#include "base/YPacket.h"

template <class inType, class outType>
class YDataProcessor : public YThread, public YAsyncQueue<inType>
{

public:

    YDataProcessor() :
        _next_processor(nullptr) {}

    virtual ~YDataProcessor()
    {
        //
    }

//    void connectOutputTo(YAsyncQueue<inType>* next_processor)
//    {
//        _next_processor = next_processor;
//    }
    virtual void connectOutputTo(YAsyncQueue<outType>* next_processor) final
    {
        _next_processor = next_processor;
    }
//    virtual void connectOutputTo(YAsyncQueue<inType>* next_processor) final
//    {
//        _next_processor = next_processor;
//    }
//    virtual void connectOutputTo(YAsyncQueue<outType>* next_processor) final
//    {
//        _next_processor = next_processor;
//    }

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
//        inType input_data;
//        if (!_input_queue.pop(input_data)) {
//            utils::sleep_for(SHORT_DELAY_MS);
//            return YCode::AGAIN;
//        }
//        return processInputData(input_data);
        return YCode::ERR;
    }

protected:


    YAsyncQueue<outType>*                _next_processor;

};
