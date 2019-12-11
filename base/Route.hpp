#pragma once
#include "base/Processor.hpp"
#include "base/Stream.hpp"

namespace fpp {

    using ProcessorSequence = ProcessorList;

    class Route : public Object {

    public:

        Route();
        virtual ~Route() override;

        virtual Code        init() override;
        virtual std::string toString() const override;

        Code                setSource(ProcessorPtr* source);
        Code                setSink(ProcessorPtr* sink);
        Code                append(ProcessorPtr processor);

        Code                setMetaRoute(int64_t input_stream_uid, int64_t output_stream_uid);

        int64_t             inputStreamUid()  const;
        int64_t             outputStreamUid() const;

        bool                contains(const int64_t uid) const;

        Code                startAll();
        Code                destroy();

        ProcessorSequence   processorSequence();

        Code                changePartTo(ProcessorVector other);

    private:

        int64_t             _input_stream_uid;
        int64_t             _output_stream_uid;
        ProcessorSequence   _sequence;
        ProcessorPtr*       _source;
        ProcessorPtr*       _sink;

    };

    using RouteList = std::list<Route>;
    using RouteVector = std::vector<Route>;

} // namespace fpp
