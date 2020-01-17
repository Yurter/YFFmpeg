#pragma once
#include "base/Processor.hpp"
#include "stream/Stream.hpp"

namespace fpp {

    using ProcessorSequence = ProcessorList;

    class Route : public Object {

    public:

        Route();
        virtual ~Route() override;

        virtual Code        init() override;
        virtual std::string toString() const override;

        Code                append(ProcessorPointer processor);

        Code                setMetaRoute(int64_t input_stream_uid, int64_t output_stream_uid);

        int64_t             inputStreamUid()  const;
        int64_t             outputStreamUid() const;

        bool                contains(const int64_t uid) const;

        Code                startAll();
        Code                destroy();

        ProcessorSequence   processorSequence();

        Code                changePartTo(ProcessorVector other);

    private:

        UID          _input_stream_uid;
        UID          _output_stream_uid;
        ProcessorSequence   _sequence;

    };

    using RouteVector = std::vector<Route>;

} // namespace fpp
