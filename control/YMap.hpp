#pragma once

#include "base/YPacket.hpp"
#include "base/YDataProcessor.hpp"
#include "../context/YSource.hpp"
#include "../context/YDestination.hpp"
#include <map>

using PacketProcessor = YAsyncQueue<YPacket>;

/* Таблица соответствий входного и выходного потоков */
using StreamMap = std::map<YStream*,YStream*>;
/* Таблица соответствий uid входного потока и локального индекса выходного потока */
using IndexMap = std::map<int64_t,int64_t>;
/* Таблица соответствий uid входного потока и указателя на первый в последовательности обработчик пакета */
using PacketMap = std::map<int64_t,PacketProcessor*>;

class YMap : public YDataProcessor<YPacket,YPacket>
{

public:

    YMap();
    virtual ~YMap() override = default;

    virtual std::string toString() const override;

    StreamMap*          streamMap();

//    YCode               setRoute(YContext* input_context, int64_t input_stream_index
//                                 , YContext* output_context, int64_t output_stream_index);
    YCode               addRoute(YStream* in_stream, YStream* out_stream);              ///< Связвание входного и выходного потоков.
    YCode               setRoute(YStream* src_stream, PacketProcessor* next_processor); ///< Связывание источника данных с первым обработчиком в последоваельности.

private:

    virtual YCode       processInputData(YPacket& input_data) override;

private:

    StreamMap           _stream_map;
    PacketMap           _packet_map;
    IndexMap            _index_map;

};
