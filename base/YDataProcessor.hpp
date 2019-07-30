#pragma once

#include "utils.hpp"
#include "YThread.hpp"
#include "YAsyncQueue.hpp"

//TODO создать родительский нешаблонный класс и от наследовать YPacketProcessor и YFrameProcessor и тд
//                                                  пакет-пакет, фрейм-фрейм, пакет-фрейм, фрейм-пакет
//              объекдинение (декодера и энкодера); (ресемплера, рескейлера, аудио- и видео-фильтров ?)
template <class inType, class outType>
class YDataProcessor : public YThread
                        , public YAsyncQueue<inType>
{

public:

    typedef YAsyncQueue<outType> NextProcessor;

    YDataProcessor() :
        _next_processor(nullptr),
        _skip_types(0),
        _ignore_types(0)
    {
        setName("YDataProcessor");
    }

    virtual ~YDataProcessor() = default;

//    void connectOutputTo(NextProcessor* next_processor)
//    {
//        _next_processor = next_processor;
//    }
    YCode connectOutputTo(YObject* next_processor)
    {
        auto ptr = dynamic_cast<NextProcessor*>(next_processor);
        return_if(not_inited_ptr(ptr), YCode::INVALID_INPUT);
        _next_processor = ptr;
        return YCode::OK;
    }

    void    setSkipType(YMediaType type) { _skip_types |= type; }
    bool    skipType(YMediaType type) { return _skip_types & type; }

    void    setIgnoreType(YMediaType type) { _ignore_types |= type; }
    bool    ignoreType(YMediaType type) { return _ignore_types &= type; }

protected:

    [[nodiscard]] virtual YCode processInputData(inType& input_data) = 0;
    [[nodiscard]] virtual YCode sendOutputData(outType output_data
                                                , NextProcessor* next_proc = nullptr) final
    {
        auto pointer = inited_ptr(next_proc) ? next_proc : _next_processor;
        guaranteed_push(pointer, output_data);
        return YCode::OK;
    }

private:

    [[nodiscard]] YCode run() override final
    {
        inType input_data;
        guaranteed_pop(this, input_data);
        return_if(ignoreType(input_data.type()), YCode::AGAIN);
        return processInputData(input_data);
    }

private:

    NextProcessor*      _next_processor;
    int64_t             _skip_types;
    int64_t             _ignore_types;

};
