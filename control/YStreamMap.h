#pragma once

#include "base/YPacket.h"
#include "base/YDataProcessor.h"
#include "../context/YSource.h"
#include "../context/YDestination.h"
#include <map>

typedef std::map<YStream*,YStream*>             stream_map;
typedef std::map<int64_t,YAsyncQueue<YPacket>*> packet_map;

class YStreamMap : public YDataProcessor<YPacket,YPacket>
{

public:

    YStreamMap();
    virtual ~YStreamMap() override = default;

    stream_map& map();

    YCode       addRoute(streams_pair streams);
    YCode       setRoute(YStream* src_stream, YAsyncQueue<YPacket>* next_processor);
    YCode       processInputData(YPacket& input_data) override;

private:

    YAsyncQueue<YPacket>* findFirstDataProcessor();

private:

    // General
    stream_map  _stream_map;
    packet_map  _route_map;

    int64_t     _source_uid;
    int64_t     _destination_uid;

};
