#pragma once
#include <fpp/media/MediaSource.hpp>
#include <fpp/media/MediaSink.hpp>
#include <tuple>
#include <stdint.h>

namespace fpp {

    using TimePoint = int64_t;
    using FileName = std::string;
    using FilePart = std::tuple<FileName,TimePoint,TimePoint>;
    using ConcatList = std::list<FilePart>;

    class Concatenator : public Thread {

    public:

        Concatenator(const std::string& output_file_name, ConcatList concat_list);
        virtual ~Concatenator() override;

    private:

        virtual void        init()  override;
        virtual Code        run()   override;

    private:

        std::string         _output_file_name;
        ConcatList          _concat_list;

    };

} // namespace fpp
