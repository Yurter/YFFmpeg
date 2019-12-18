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
        ProcessorPointer output_file = std::make_shared<MediaSink>(_output_file_name, IOType::Event); /* crutch: preset */
        try_to(output_file->init());

        output_file->stream(0)->setStampType(StampType::Append);

        { /* crutch */
            log_info("Initing stream");
            MediaSource input_file(std::get<0>(*_concat_list.begin()));
            try_to(input_file.init());
            try_to(input_file.open());

            try_to(output_file->stream(0)->params->completeFrom(input_file.stream(0)->params));
            output_file->stream(0)->setUsed(true);
            try_to(input_file.close());
            input_file.join();
            log_info("Stream inited");
        }

        try_to(output_file->open());
        try_to(output_file->start());

        int64_t out_duration = 0;
        int64_t old_out_duration = 0;
        std::ofstream file_crutch;
        file_crutch.open(_output_file_name + ".txt");
        std::ofstream file_crutch2;
        file_crutch2.open(_output_file_name + "2.txt");
        std::ofstream file_crutch3;
        file_crutch3.open(_output_file_name + "3.txt");

        for (auto& [input_file_name, start_point, end_point] : _concat_list) {
            log_debug("File: " << input_file_name << ", "
                      << "from " << (start_point == FROM_START ? "start " : std::to_string(start_point) + "ms ")
                      << "to " << (end_point == FROM_START ? "end" : std::to_string(end_point) + "ms "));
            file_crutch3 << "File: " << input_file_name << ", "
                         << "from " << (start_point == FROM_START ? "start " : std::to_string(start_point) + "ms ")
                         << "to " << (end_point == FROM_START ? "end" : std::to_string(end_point) + "ms ") << std::endl;

            MediaSource input_file(input_file_name);
            input_file.setStartPoint(start_point);
            input_file.setEndPoint(end_point);
            input_file.doNotSendEOF(); /* crutch */
            try_to(input_file.init());
            try_to(input_file.open());
            input_file.stream(0)->setUsed(true); /* crutch */

            try_to(input_file.connectTo(output_file));
            try_to(input_file.start());
            input_file.join();
            try_to(input_file.disconnectFrom(output_file));

//            while (out_duration == output_file.stream(0)->parameters->duration()) {
//                utils::sleep_for_ms(50);
//            }

//            file_crutch << input_file_name << '\t'
//                        << out_duration + (start_point == FROM_START ? 0 : start_point) << '\t'
//                        << out_duration + (end_point == TO_END ? input_file.stream(0)->parameters->duration() : end_point) << '\n';
            out_duration = output_file->stream(0)->params->duration();

            file_crutch << input_file_name << '\t'
                        << old_out_duration << '\t'
                        << out_duration << '\n';

            old_out_duration = out_duration;

            file_crutch2 << "out_duration = " << out_duration << '\n';

            Code ret = input_file.exitCode();
            return_if(utils::error_code(ret), ret);
        }

        Packet eof_packet;
        eof_packet.setType(MediaType::MEDIA_TYPE_VIDEO);
        try_to(output_file->push(&eof_packet));

        file_crutch.close();

        return Code::END_OF_FILE;
    }

} // namespace fpp
