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
        MediaSink output_file(_output_file_name, IOType::Event);
        try_to(output_file.init());
        try_to(output_file.open());
        for (auto& [input_file_name, start_point, end_point] : _concat_list) {
            MediaSource input_file(input_file_name);
        }
    }

} // namespace fpp
