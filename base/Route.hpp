#pragma once
#include "base/Processor.hpp"

namespace fpp {

    class Route : public Object {

    public:

        Route();
        virtual ~Route() override;

        virtual Code        init() override;
        virtual std::string toString() const override;

        Code                append(Processor* processor);

    private:

        using ProcessorSequence = std::vector<Processor*>;

        int64_t             _input_stream_uid;
        int64_t             _output_stream_uid;
        ProcessorSequence   _sequence;

};

} // namespace fpp
