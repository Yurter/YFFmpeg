#pragma once
#include "base/Processor.hpp"
#include "base/Stream.hpp"

namespace fpp {

    using ProcessorSequence = ProcessorVector;

    class Route : public Object {

    public:

        Route();
//        Route(const Route& other);
//        Route(const Route&& other);
        virtual ~Route() override;

//        Route& operator=(const Route&) = delete;

        virtual Code        init() override;
        virtual std::string toString() const override;

        Code                setMetaRoute(int64_t input_stream_uid, int64_t output_stream_uid);
        Code                append(ProcessorPtr* processor);

        int64_t             inputStreamUid()  const;
        int64_t             outputStreamUid() const;

        bool                contains(const Processor * const processor) const;

        Code                startAll();
        Code                destroy();

        ProcessorSequence   processorSequence();

        Code                changePartTo(ProcessorVector other);

    private:

        int64_t             _input_stream_uid;
        int64_t             _output_stream_uid;
        ProcessorSequence   _sequence;

    };

    using RouteList = std::list<Route>;
    using RouteVector = std::vector<Route>;

} // namespace fpp
