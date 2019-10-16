#include "YMap.hpp"
#include <algorithm>
#include <iterator>

namespace fpp {

    YMap::YMap()
    {
        setName("YMap");
    }

    std::string YMap::toString() const {
        return_if(_stream_map.empty(), "Empty");
        std::string str;
        for (auto&& route : _stream_map) {
            auto in_stream_id = route.first->index();
            auto out_stream_id = route.second->index();
            auto in_context_uid = utils::get_context_uid(route.first->uid());
            auto out_context_uid = utils::get_context_uid(route.second->uid());
            str += "[" + std::to_string(in_context_uid)
                    + ":"
                    + std::to_string(in_stream_id) + "]";
            str += "-->";
            str += "[" + std::to_string(out_context_uid)
                    + ":"
                    + std::to_string(out_stream_id) + "]";
            str += " ";
        }
        return str;
    }

    StreamMap* YMap::streamMap() {
        return &_stream_map;
    }

    //Code YMap::setRoute(Context* input_context, int64_t input_stream_index
    //                     , Context* output_context, int64_t output_stream_index) {
    //    return_if(not_inited_ptr(input_context), Code::INVALID_INPUT);
    //    return_if(not_inited_ptr(output_context), Code::INVALID_INPUT);
    //    return_if(input_stream_index < 0, Code::INVALID_INPUT);
    //    return_if(output_stream_index < 0, Code::INVALID_INPUT);

    //    return Code::OK;
    //}

    Code YMap::addRoute(Stream* in_stream, Stream* out_stream) {
        return_if(in_stream->type() != out_stream->type(), Code::INVALID_INPUT);
        return_if(in_stream->uid() == out_stream->uid(),   Code::INVALID_INPUT);
        _stream_map.insert({ in_stream, out_stream });
        _index_map.insert({ in_stream->uid(), out_stream->parameters->streamIndex() });
        return Code::OK;
    }

    Code YMap::setRoute(Stream* src_stream, PacketProcessor* next_processor) {
        return_if(invalid_int(src_stream->uid()), Code::INVALID_INPUT);
        _packet_map.insert({src_stream->uid(), next_processor});
        setInited(true);
        return Code::OK;
    }

    Code YMap::processInputData(Packet& input_data) {
        /* Определение локального индекса выходного потока */
        int64_t out_stream_index = INVALID_INT;
        try {
//            log_info("Trying 1 " << input_data);
            out_stream_index = _index_map.at(input_data.streamUid());
        } catch (std::out_of_range) { return Code::INVALID_INPUT; }

        /* Определение первого обработчика пакета */
        NextProcessor* next_proc = nullptr;
        try {
//            log_info("Trying 2");
            next_proc = _packet_map.at(input_data.streamUid());
        } catch (std::out_of_range) { return Code::INVALID_INPUT; }

        /* Инициализаця индекса потока и отправка пакета */
        input_data.setStreamIndex(out_stream_index);
        return sendOutputData(input_data, next_proc);
    }

} // namespace fpp
