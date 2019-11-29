#pragma once
#include "core/Thread.hpp"
#include "Stream.hpp"

namespace fpp {

    class Processor : public Thread {

    public:

        Processor();
        virtual ~Processor() override;

        virtual Code        open();
        virtual Code        close();
        virtual Code        push(const Object* input_data) = 0;

        bool                opened() const;
        bool                closed() const;

        uint64_t            uid() const;

        StreamVector        streams();
        Stream*             stream(int64_t index);
        Code                addStream(Stream* stream);

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

        uint64_t            _uid;
        bool                _opened;
        bool                _close_on_disconnect;
        int64_t             _skip_types;
        int64_t             _discard_types;
        StreamVector        _streams;

    };

    using ProcessorVector = std::vector<Processor*>;
    using ProcessorList = std::list<Processor*>;

} // namespace fpp
