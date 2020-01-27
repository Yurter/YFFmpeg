#pragma once
#include <base/Filter.hpp>

namespace fpp {

    class AudioFilter : public Filter {

    public:

        AudioFilter(SharedParameters parameters, std::string filters_descr);

    private:

        virtual Code processInputData(Frame input_data) override;

    };

} // namespace fpp
