#pragma once
#include "Thread.hpp"

namespace fpp {

    class Processor : public Thread {

    public:

        Processor();
        virtual ~Processor() override;

        virtual Code open()  = 0;
        virtual Code close() = 0;

        virtual Code push()  = 0;

        Code                connectTo(const Processor* other);

        void                setSkipType(MediaType type);
        bool                skipType(MediaType type) const;

        void                setDiscardType(MediaType type);
        bool                discardType(MediaType type) const;

    private:

        const Processor*    _next_processor;
        int64_t             _skip_types;
        int64_t             _discard_types;

    };

} // namespace fpp
