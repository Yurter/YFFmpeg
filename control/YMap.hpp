#pragma once
#include "base/Packet.hpp"
#include "base/TemplateProcessor.hpp"
#include "../context/MediaSource.hpp"
#include "../context/MediaSink.hpp"
#include <map>

namespace fpp {

    using PacketProcessor = AsyncQueue<Packet>;

    /* Таблица соответствий входного и выходного потоков */
    using StreamMap = std::multimap<Stream*,Stream*>;
    /* Таблица соответствий uid входного потока и локального индекса выходного потока */
    using IndexMap = std::multimap<int64_t,int64_t>;
    /* Таблица соответствий uid входного потока и указателя на первый в последовательности обработчик пакета */
    using PacketMap = std::multimap<int64_t,PacketProcessor*>;

    class YMap : public TemplateProcessor<Packet,Packet> {

    public:

        YMap();
        virtual ~YMap() override = default;

        virtual std::string toString() const override;

        StreamMap*          streamMap();

    //    Code               setRoute(Context* input_context, int64_t input_stream_index
    //                                 , Context* output_context, int64_t output_stream_index);
        Code                addRoute(Stream* in_stream, Stream* out_stream);              ///< Связвание входного и выходного потоков.
        Code                setRoute(Stream* src_stream, PacketProcessor* next_processor); ///< Связывание источника данных с первым обработчиком в последоваельности.

    private:

        virtual Code        processInputData(Packet& input_data) override;

        Code                checkInputs();

    private:

        StreamMap           _stream_map;
        PacketMap           _packet_map;
        IndexMap            _index_map;

    };

} // namespace fpp
