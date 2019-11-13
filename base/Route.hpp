#pragma once
#include "base/Processor.hpp"
#include "base/Stream.hpp"

namespace fpp {

    class Route : public Object {

    public:

        Route();
        virtual ~Route() override;

        virtual Code        init() override;
        virtual std::string toString() const override;

        Code                setMetaRoute(int64_t input_stream_uid, int64_t output_stream_uid);
        Code                append(Processor* processor);

        int64_t             inputStreamUid() const;
        int64_t             outputStreamUid() const;

    private:

        using ProcessorSequence = ProcessorVector;

        int64_t             _input_stream_uid;
        int64_t             _output_stream_uid;
        ProcessorSequence   _sequence;

    };

    using RouteList = std::list<Route>;

} // namespace fpp
