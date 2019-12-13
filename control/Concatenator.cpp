#include "Concatenator.hpp"

namespace fpp {

    Concatenator::Concatenator(const std::string& output_file_name
                               , ConcatList concat_list) :
        _output_file_name(output_file_name)
    , _concat_list(concat_list)
    {
        setName("Concatenator");
    }

    Concatenator::~Concatenator() {
        try_throw(stop());
        join();
    }

    Code Concatenator::init() {
        return_if(_concat_list.size() < 2, Code::INVALID_INPUT);
        return Code::OK;
    }

    Code Concatenator::run() {
        MediaSink output_file(_output_file_name, IOType::Event); /* crutch: preset */
        try_to(output_file.init());

        { /* crutch */
            log_info("Initing stream");
            MediaSource input_file(std::get<0>(*_concat_list.begin()));
            try_to(input_file.init());
            try_to(input_file.open());

            try_to(output_file.stream(0)->parameters->completeFrom(input_file.stream(0)->parameters));
            output_file.stream(0)->setUsed(true);
            try_to(input_file.close());
            input_file.join();
            log_info("Stream inited");
        }

        try_to(output_file.open());
        try_to(output_file.start());

        for (auto& [input_file_name, start_point, end_point] : _concat_list) {
//            log_info("input_file_name: " << input_file_name);
//            log_info("start_point: " << start_point);
//            log_info("end_point: " << end_point);
            log_debug("File: " << input_file_name << ", "
                      << "from " << (start_point == FROM_START ? "start" : std::to_string(start_point) + "ms ")
                      << "to " << (end_point == FROM_START ? "end" : std::to_string(end_point) + "ms "));

            MediaSource input_file(input_file_name);
            input_file.setStartPoint(start_point);
            input_file.setEndPoint(end_point);
            input_file.doNotSendEOF(); /* crutch */
            try_to(input_file.init());
            try_to(input_file.open());
            input_file.stream(0)->setUsed(true); /* crutch */

            try_to(input_file.connectTo(&output_file));
            try_to(input_file.start());
            input_file.join();
            try_to(input_file.disconnectFrom(&output_file));
//            log_info("iter finished");
//            utils::sleep_for_ms(150);
        }

        Packet eof_packet;
        eof_packet.setType(MediaType::MEDIA_TYPE_VIDEO);
        output_file.push(&eof_packet);
//        output_file.join();

//        try_to(output_file.close());
//        try_to(output_file.stop());
//        output_file.join();

        return Code::END_OF_FILE;
    }

} // namespace fpp
