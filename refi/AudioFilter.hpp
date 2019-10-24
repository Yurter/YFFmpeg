#pragma once
#include "Filter.hpp"

namespace fpp {

    class AudioFilter : public Filter {

    public:

        AudioFilter(const Parameters* input_data_params, std::string filters_descr);

    private:

        virtual Code processInputData(Frame input_data) override;

    };

} // namespace fpp
