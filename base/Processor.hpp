#pragma once
#include "core/Thread.hpp"
#include "Stream.hpp"

namespace fpp {

    class Processor : public Thread {

    public:

        Processor();
        Processor(Processor& other) = delete;
        Processor(Processor&& other) = delete;
        virtual ~Processor() override;

        virtual Code        open();
        virtual Code        close();
        virtual Code        push(const Object* input_data) = 0;

        int64_t             uid() const;
        StreamVector        streams();
        Stream*             stream(int64_t index) const;
        Code                addStream(Stream* stream);
        Code                setStreams(StreamVector streams);

        void                setOpened(bool opened);
        void                setCloseOnDisconnect(bool value);
        void                setDiscardType(MediaType type);
        void                setMetaData(const std::string& value);

        Code                connectTo(Processor* other);
        Code                disconnectFrom(Processor* other);

        virtual bool        equalTo(const Processor * const other);

        bool                opened() const;
        bool                closed() const;
        bool                discardType(MediaType type) const;
        std::string         metaData() const;

    protected:

        using ProcessorList = std::list<Processor*>;
        ProcessorList       _next_processor_list;

    private:

        const int64_t       _uid;
        bool                _opened;
        bool                _close_on_disconnect;
        int64_t             _discard_types;
        StreamVector        _streams;
        std::string         _meta_data;

    };

    using ProcessorVector = std::vector<Processor*>;
    using ProcessorList = std::list<Processor*>;

} // namespace fpp
