#pragma once
#include "core/Thread.hpp"

namespace fpp {

    class Processor : public Thread {

    public:

        Processor(ProcessorType type);
        virtual ~Processor() override;

        virtual Code        open();
        virtual Code        close();
        virtual Code        push(const Object* input_data) = 0;

        bool                opened() const;
        bool                closed() const;

        ProcessorType       type() const;
        bool                typeIs(ProcessorType value) const;

        void                setOpened(bool opened);

        Code                connectTo(Processor* other);
        Code                disconnectFrom(Processor* other);

        void                setCloseOnDisconnect(bool value);

        void                setSkipType(MediaType type);
        bool                skipType(MediaType type) const;

        void                setDiscardType(MediaType type);
        bool                discardType(MediaType type) const;

    protected:

        using ProcessorList = std::list<Processor*>;
        ProcessorList       _next_processor_list;

    private:

        bool                _opened;
        bool                _close_on_disconnect;
        ProcessorType       _type;
        int64_t             _skip_types;
        int64_t             _discard_types;

    };

    using ProcessorVector = std::vector<Processor*>;
    using ProcessorList = std::list<Processor*>;

} // namespace fpp
