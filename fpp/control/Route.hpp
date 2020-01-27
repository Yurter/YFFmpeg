#pragma once
#include <fpp/base/Processor.hpp>
#include <fpp/stream/Stream.hpp>

namespace fpp {

    using ProcessorSequence = ProcessorList;

    class Route : public Object {

    public:

        Route();
        virtual ~Route() override;

        virtual void        init() override;
        virtual std::string toString() const override;

        Code                append(SharedProcessor processor);

        Code                setMetaRoute(int64_t input_stream_uid, int64_t output_stream_uid);

        int64_t             inputStreamUid()  const;
        int64_t             outputStreamUid() const;

        bool                contains(const int64_t uid) const;

        Code                startAll();
        Code                destroy();

        ProcessorSequence   processorSequence();

        Code                changePartTo(ProcessorVector other);

    private:

        uid_t               _input_stream_uid;
        uid_t               _output_stream_uid;
        ProcessorSequence   _sequence;

    };

    using RouteVector = std::vector<Route>;

} // namespace fpp