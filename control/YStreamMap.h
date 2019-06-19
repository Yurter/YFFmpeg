#pragma once

#include "base/YPacket.h"
#include "base/YDataProcessor.h"
#include "../context/YSource.h"
#include "../context/YDestination.h"
#include <map>

//typedef std::map<YStream*,YAsyncQueue<YPacket>> YMap;
typedef std::map<int64_t,YAsyncQueue<YPacket>*> YMap;

class YStreamMap : public YDataProcessor<YPacket,YPacket>
{

public:

    YStreamMap();
    virtual ~YStreamMap() override = default;

    YCode       addRoute(YStream* src_stream, YStream* dst_stream);

    bool        addRoute(YStream* src_stream, YAsyncQueue<YPacket>* next_processor);
    YCode       processInputData(YPacket& input_data) override;

private:

    YAsyncQueue<YPacket>* findFirstDataProcessor();

private:

    // General
    YMap        _route_map;
    int64_t     _source_uid;

};
