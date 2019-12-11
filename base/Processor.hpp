#pragma once
#include "core/AsyncList.hpp"
#include "core/Thread.hpp"
#include "Stream.hpp"

namespace fpp {

    class Processor;
    using ProcessorPointer = std::shared_ptr<Processor>;
    using ProcessorVector = std::vector<ProcessorPointer>; //TODO не потокобезопасен
    using ProcessorList = AsyncList<ProcessorPointer>;

    class Processor : public Thread {

    public:

        Processor();
        Processor(const Processor& other) = delete;
        Processor(const Processor&& other) = delete;
        Processor& operator=(const Processor& other) = delete;
        Processor& operator=(const Processor&& other) = delete;
        virtual ~Processor() override;

        virtual Code        open();
        virtual Code        close();
        virtual Code        push(const Object* input_data)/* { return Code::NOT_IMPLEMENTED; } //*/= 0;

        int64_t             uid() const;
        const StreamVector& streams() const;
        Stream*             stream(int64_t index) const;
        Code                addStream(Stream* stream);
        Code                setStreams(StreamVector streams);

        void                setType(ProcessorType value);
        void                setOpened(bool opened);
        void                setCloseOnDisconnect(bool value);
        void                setDiscardType(MediaType type);
        void                setMetaData(const std::string& value);

        Code                connectTo(const ProcessorPointer other);
        Code                disconnectFrom(const ProcessorPointer other);

        virtual bool        equalTo(const ProcessorPointer other) const;

        ProcessorType       type() const;
        bool                typeIs(ProcessorType value) const;
        bool                opened() const;
        bool                closed() const;
        bool                discardType(MediaType type) const;
        std::string         metaData() const;

    protected:

        ProcessorList       _next_processor_list;

    private:

        const int64_t       _uid;
        ProcessorType       _type;
        bool                _opened;
        bool                _close_on_disconnect;
        int64_t             _discard_types;
        StreamVector        _streams;
        std::string         _meta_data;

    };

} // namespace fpp
