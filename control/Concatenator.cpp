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
    }

    Code Concatenator::init() {
        return_error_if(_concat_list.size() < 1
                        , "concat list can't be empty"
                        , Code::INVALID_INPUT);
        return Code::OK;
    }

    Code Concatenator::run() {
        ProcessorPointer output_file = std::make_shared<MediaSink>(_output_file_name, IOType::Event); /* crutch: preset */
        try_to(output_file->init());

//        output_file->stream(0)->setStampType(StampType::Append);
        output_file->setDiscardType(MediaType::MEDIA_TYPE_EOF);

//        log_debug("OUT UID: " << output_file->stream(0)->params->streamUid());

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

        int i = 0;
        for (auto& [input_file_name, start_point, end_point] : _concat_list) {
            log_debug("File: " << input_file_name << ", "
                      << "from " << (start_point == FROM_START ? "start " : std::to_string(start_point) + " ms ")
                      << "to " << (end_point == TO_END ? "end" : std::to_string(end_point) + " ms "));
            file_crutch3 << "File: " << input_file_name << ", "
                         << "from " << (start_point == FROM_START ? "start " : utils::time_to_string(start_point, DEFAULT_TIME_BASE) + " ms ")
                         << "to " << (end_point == TO_END ? "end" : utils::time_to_string(end_point, DEFAULT_TIME_BASE) + " ms ") << std::endl;

            MediaSource input_file(input_file_name);
            try_to(input_file.init());
            try_to(input_file.open());
            input_file.stream(0)->setUsed(true); /* crutch */
            input_file.stream(0)->setStartTimePoint(start_point);
            input_file.stream(0)->setEndTimePoint(end_point);

            try_to(input_file.connectTo(output_file));
            try_to(input_file.start());
            input_file.join();
            try_to(input_file.disconnectFrom(output_file));

            out_duration = av_rescale_q(output_file->stream(0)->params->duration(), output_file->stream(0)->params->timeBase(), DEFAULT_TIME_BASE);

            file_crutch << input_file_name << '\t'
                        << old_out_duration << '\t'
                        << out_duration << '\n';

            old_out_duration = out_duration;

            file_crutch2 << "out_duration = " << utils::time_to_string(out_duration, DEFAULT_TIME_BASE) << '\n';

            log_error(i++ << " ===========================================================");
        }

        output_file->resetDiscardTypes();
        Packet eof;
        eof.setType(MediaType::MEDIA_TYPE_EOF);
        try_to(output_file->push(&eof));

        output_file->join();

        file_crutch.close();
        return Code::END_OF_FILE;
    }

} // namespace fpp
