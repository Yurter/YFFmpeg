#pragma once
#include "media/MediaSource.hpp"
#include "media/MediaSink.hpp"
#include <tuple>

#define FROM_START  -1
#define TO_END      -1

namespace fpp {

    using ConcatList = std::list<std::tuple<std::string,int64_t,int64_t>>;

    class Concatenator : public Thread {

    public:

        Concatenator(const std::string& output_file_name, ConcatList concat_list);
        virtual ~Concatenator() override;

    private:

        virtual Code        init()  override;
        virtual Code        run()   override;

    private:

        std::string         _output_file_name;
        ConcatList          _concat_list;

    };

} // namespace fpp
