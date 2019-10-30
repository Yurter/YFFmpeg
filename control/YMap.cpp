#include "YMap.hpp"
#include <algorithm>
#include <iterator>

namespace fpp {

    YMap::YMap() {
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

    Code YMap::setRoute(Stream* src_stream, Processor* next_processor) {
        return_if(invalid_int(src_stream->uid()), Code::INVALID_INPUT);
        _packet_map.insert({ src_stream->uid(), next_processor });
        setInited(true);
        return Code::OK;
    }

    Code YMap::processInputData(Packet input_data) {
        log_info("work");
        try {
            auto out_index_list = _index_map.equal_range(input_data.streamUid());
            return_if(out_index_list.first == _index_map.end(), Code::INVALID_INPUT);

            for (auto it = out_index_list.first; it != out_index_list.second; it++) {
                /* Определение локального индекса выходного потока */
                int64_t out_stream_index = it->second;

                /* Определение следующего обработчика пакета */
                auto next_proc_list = _packet_map.equal_range(input_data.streamUid());
                return_if(next_proc_list.first == _packet_map.end(), Code::INVALID_INPUT);

                for (auto it2 = next_proc_list.first; it2 != next_proc_list.second; it2++) {
                    Processor* next_proc = it2->second;

                    /* Инициализаця индекса потока и отправка пакета */
                    input_data.setStreamIndex(out_stream_index);
                    try_to(sendOutputData(input_data, next_proc));
                }
            }
        } catch (std::out_of_range) {
            log_error("First try failed: " << input_data);
            return Code::INVALID_INPUT;
        }

        return checkInputs();
    }

//    Code YMap::processInputData(Packet& input_data) {
//        /* Определение локального индекса выходного потока */
//        int64_t out_stream_index = INVALID_INT;
//        try {
////            out_stream_index = _index_map.at(input_data.streamUid());
//            auto result = _index_map.equal_range(input_data.streamUid());

//            for (auto&& )

//            for (auto it = result.first; it != result.second; it++) {
//                std::cout << it->second << std::endl;333
//            }
//        } catch (std::out_of_range) {
//            log_error("First try failed: " << input_data);
//            return Code::INVALID_INPUT;
//        }

//        /* Определение первого обработчика пакета */
//        NextProcessor* next_proc = nullptr;
//        try {
//            next_proc = _packet_map.at(input_data.streamUid());
//        } catch (std::out_of_range) {
//            log_error("Second try failed");
//            return Code::INVALID_INPUT;
//        }

//        /* Инициализаця индекса потока и отправка пакета */
//        input_data.setStreamIndex(out_stream_index);
//        try_to(sendOutputData(input_data, next_proc));
//        return checkInputs();
//    }

    Code YMap::checkInputs() {
        return_if_not(buferIsEmpty(), Code::OK);
        bool all_stream_dead = true;
        for (auto&& [in_stream, out_stream] : _stream_map) {
            if (in_stream->used()) {
                all_stream_dead = false;
                break;
            }
        }
        return_if(all_stream_dead, Code::END_OF_FILE);
        return Code::OK;
    }
//    Code YMap::processInputData(Packet& input_data) {
//        /* Определение локального индекса выходного потока */
//        int64_t out_stream_index = INVALID_INT;
//        try {
//            out_stream_index = _index_map.at(input_data.streamUid());
//        } catch (std::out_of_range) {
//            log_error("First try failed: " << input_data);
//            return Code::INVALID_INPUT;
//        }

//        /* Определение первого обработчика пакета */
//        NextProcessor* next_proc = nullptr;
//        try {
//            next_proc = _packet_map.at(input_data.streamUid());
//        } catch (std::out_of_range) {
//            log_error("Second try failed");
//            return Code::INVALID_INPUT;
//        }

//        /* Инициализаця индекса потока и отправка пакета */
//        input_data.setStreamIndex(out_stream_index);
//        return sendOutputData(input_data, next_proc);
//    }

} // namespace fpp
