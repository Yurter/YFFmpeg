#pragma once

#include "base/YPacket.h"
#include "base/YDataProcessor.h"
#include "../context/YSource.h"
#include "../context/YDestination.h"
#include <map>

/* Таблица соответствий входного и выходного потоков */
typedef std::map<YStream*,YStream*>             stream_map;

/* Таблица соответствий uid входного потока и указателей на первый в последовательности обработчик пакета */
typedef std::map<int64_t,YAsyncQueue<YPacket>*> packet_map;

/* Таблица соответствий uid входного потока и локального индекса выходного потока */
typedef std::map<int64_t,int64_t>               index_map;

class YStreamMap : public YDataProcessor<YPacket,YPacket>
{

public:

    YStreamMap();
    virtual ~YStreamMap() override = default;

    stream_map&         map();

    YCode               addRoute(media_stream source, media_stream destination);
    YCode               setRoute(YStream* src_stream, YAsyncQueue<YPacket>* next_processor);

private:

    YCode               checkStreamPair(streams_pair streams);
    YCode               processInputData(YPacket& input_data) override;

private:

    // General
    stream_map          _stream_map;
    packet_map          _packet_map;
    index_map           _index_map;

    int64_t             _source_uid;
    int64_t             _destination_uid;

};
