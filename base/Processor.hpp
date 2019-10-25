#pragma once
#include "core/Thread.hpp"

namespace fpp {

    class Processor : public Thread {

    public:

        Processor();
        virtual ~Processor() override;

        virtual Code        open()  = 0;
        virtual Code        close() = 0;
        virtual Code        push(const Object* input_data) = 0;

        bool                opened() const;
        bool                closed() const;

        void                setOpened(bool opened);

        Code                connectTo(Processor* other);

        void                setSkipType(MediaType type);
        bool                skipType(MediaType type) const;

        void                setDiscardType(MediaType type);
        bool                discardType(MediaType type) const;

    protected:

        Processor*          _next_processor;

    private:

        int64_t             _skip_types;
        int64_t             _discard_types;

    };

} // namespace fpp
