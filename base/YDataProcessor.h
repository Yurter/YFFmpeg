#pragma once

#include "ffmpeg.h"
#include "utils.h"
#include "YThread.h"
#include "YAsyncQueue.h"
#include "YData.h"
#include "YPacket.h"

template <class inType, class outType>
class [[nodiscard]] YDataProcessor : public YThread
                                    , public YAsyncQueue<inType>
{

public:

    YDataProcessor() :
        _next_processor(nullptr),
        _last_error(YCode::OK),
        _inited(false),
        _skip_types(0),
        _ignore_types(0)
    {
        setName("YDataProcessor");
    }

    virtual ~YDataProcessor() = default;

    virtual void connectOutputTo(YAsyncQueue<outType>* next_processor) final
    {
        _next_processor = next_processor;
    }

    void    setLastError(YCode last_error) { _last_error = last_error; }
    YCode   lastError() const { return _last_error; }

    void    setInited(bool inited) { _inited = inited; }
    bool    inited() const { return _inited; }

    void    setSkipType(YMediaType type) { _skip_types |= type; }
    bool    skipType(YMediaType type) { return _skip_types & type; }

    void    setIgnoreType(YMediaType type) { _ignore_types |= type; }
    bool    ignoreType(YMediaType type) { return _ignore_types &= type; }

protected:

    virtual YCode processInputData(inType& input_data) = 0;
    virtual YCode sendOutputData(outType output_data) final
    {
        if (_next_processor == nullptr) {
            setLastError(YCode::NOT_INITED);
            return lastError();
        }
        _next_processor->push(output_data);
        return YCode::OK;
    }

private:

    YCode run() override final
    {
        if (!inited()) {
            return YCode::NOT_INITED;
        }
        inType input_data;
        if (!pop(input_data)) {
            utils::sleep_for(SHORT_DELAY_MS);
            return YCode::AGAIN;
        }
        if (ignoreType(input_data.type())) {
            return YCode::AGAIN;
        }
        setLastError(processInputData(input_data));
        return lastError();
    }

//protected:
private:


    YAsyncQueue<outType>*   _next_processor;

    YCode       _last_error;
    bool        _inited;

    int64_t     _skip_types;
    int64_t     _ignore_types;

};
