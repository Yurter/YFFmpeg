#pragma once

#include "utils.h"
#include "YThread.h"
#include "YAsyncQueue.h"

template <class inType, class outType>
class YDataProcessor : public YThread
                        , public YAsyncQueue<inType>
{

public:

    typedef YAsyncQueue<outType> YNextProcessor;

    YDataProcessor() :
        _next_processor(nullptr),
        _inited(false),
        _skip_types(0),
        _ignore_types(0)
    {
        setName("YDataProcessor");
    }

    virtual ~YDataProcessor() = default;

    virtual void connectOutputTo(YNextProcessor* next_processor) final
    {
        _next_processor = next_processor;
    }

    void    setInited(bool inited) { _inited = inited; }
    bool    inited() const { return _inited; }

    void    setSkipType(YMediaType type) { _skip_types |= type; }
    bool    skipType(YMediaType type) { return _skip_types & type; }

    void    setIgnoreType(YMediaType type) { _ignore_types |= type; }
    bool    ignoreType(YMediaType type) { return _ignore_types &= type; }

protected:

    [[nodiscard]] virtual YCode processInputData(inType& input_data) = 0;
    [[nodiscard]] virtual YCode sendOutputData(outType output_data
                                                , YNextProcessor* next_proc = nullptr) final
    {
        if (next_proc != nullptr) {
            guaranteed_push(next_proc, output_data);
            return YCode::OK;
        }
        return_if(not_inited_ptr(_next_processor), YCode::NOT_INITED); //TODO убрать
        guaranteed_push(_next_processor, output_data);
        return YCode::OK;
    }

private:

    [[nodiscard]] YCode run() override final
    {
        return_if(!inited(), YCode::NOT_INITED); //TODO убрать
        inType input_data;
        if (!pop(input_data)) {
            utils::sleep_for(SHORT_DELAY_MS);
            return YCode::AGAIN;
        }
        return_if(ignoreType(input_data.type()), YCode::AGAIN);
        return processInputData(input_data);
    }

private:

    // General
    bool                    _inited;

    // Media
    YAsyncQueue<outType>*   _next_processor;
    int64_t                 _skip_types;
    int64_t                 _ignore_types;

};
