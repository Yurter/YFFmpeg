#pragma once

#include "utils.hpp"
#include "Thread.hpp"
#include "AsyncQueue.hpp"

//TODO создать родительский нешаблонный класс и от наследовать PacketProcessor и FrameProcessor и тд
//                                                  пакет-пакет, фрейм-фрейм, пакет-фрейм, фрейм-пакет
//              объекдинение (декодера и энкодера); (ресемплера, рескейлера, аудио- и видео-фильтров ?)

namespace fpp {

    template <class inType, class outType>
    class DataProcessor : public Thread
                            , public AsyncQueue<inType> {

    public:

        using NextProcessor = AsyncQueue<outType>;

        DataProcessor() :
            _next_processor(nullptr),
            _skip_types(0),
            _ignore_types(0)
        {
            setName("DataProcessor");
        }

        virtual ~DataProcessor() = default;

        Code connectOutputTo(Object* next_processor) {
            auto ptr = dynamic_cast<NextProcessor*>(next_processor);
            return_if(not_inited_ptr(ptr), Code::INVALID_INPUT);
            _next_processor = ptr;
            return Code::OK;
        }

        void    setSkipType(MediaType type) { _skip_types |= type; }
        bool    skipType(MediaType type) { return _skip_types & type; }

        void    setIgnoreType(MediaType type) { _ignore_types |= type; }
        bool    ignoreType(MediaType type) { return _ignore_types &= type; }

    protected:

        virtual Code processInputData(inType& input_data) = 0;

        Code sendOutputData(const outType& output_data, NextProcessor* next_proc = nullptr) {
            auto pointer = inited_ptr(next_proc) ? next_proc : _next_processor;
            return_if(not_inited_ptr(pointer), Code::ERR);
            if (this->is("Rescaler") || this->is("Decoder h264")) {
                if (output_data.empty()) {
                    log_error("Send: " << output_data << " -> " << output_data.empty());
                }
            }
//            log_warning("Send: " << output_data);
//            if (output_data.empty()) {
//                log_error("Send empty: " << output_data << " + " << output_data.empty());
//            }
            return_if_not(pointer->wait_and_push(output_data), Code::EXIT);
            return Code::OK;
        }

    private:

        virtual Code run() override final {
            inType input_data;
            return_if_not(wait_and_pop(input_data), Code::EXIT);
//            log_warning("Poped: " << input_data);
            if (this->is("Rescaler") || this->is("Decoder h264")) {
                if (input_data.empty()) {
                    log_error("Poped: " << input_data << " -> " << input_data.empty());
                }
            }
            return_if(ignoreType(input_data.type()), Code::AGAIN);
            if (input_data.empty() && !this->is("YMap") && !this->is("Source")) {
                log_warning("Sending empty data: " << outType());
                sendOutputData(outType());
                return Code::END_OF_FILE;
            }
            return processInputData(input_data);
        }

    private:

        NextProcessor*      _next_processor;
        int64_t             _skip_types;
        int64_t             _ignore_types;

    };

} // namespace fpp
