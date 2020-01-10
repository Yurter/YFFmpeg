#pragma once
#include "core/async/AsyncList.hpp"
#include "core/Thread.hpp"
#include "Stream.hpp"
#include <map>

namespace fpp {

    class Processor;
    using ProcessorPointer = std::shared_ptr<Processor>;
    using ProcessorVector = std::vector<ProcessorPointer>; //TODO не потокобезопасен

    using ProcessorList = AsyncList<ProcessorPointer>;
    using StreamMap = std::map<StreamId_t,ProcessorList>;

    class Processor : public Thread {

    public:

        Processor();
        Processor(const Processor& other)  = delete;
        Processor(const Processor&& other) = delete;
        Processor& operator=(const Processor& other)  = delete;
        Processor& operator=(const Processor&& other) = delete;
        virtual ~Processor() override;

        virtual Code        open();
        virtual Code        close();
        virtual Code        push(const Object* input_data) = 0;

        int64_t             uid()                       const;
        const StreamVector& streams()                   const;
        Stream*             bestStream(MediaType type)  const;
        Stream*             stream(int64_t index)       const;
        Code                addStream(Stream* stream);
        Code                setStreams(StreamVector streams);

        void                setType(ProcessorType value);
        void                setOpened(bool opened);
        void                setCloseOnDisconnect(bool value);
        void                setDiscardType(MediaType type);
        void                resetDiscardTypes() { _discard_types = 0; } //TODO костыль: нельзя закрыть синк еоф пакетом в конкатенаторе
        void                setMetaData(const std::string& value);

        Code                connectTo(StreamId_t stream_id, ProcessorPointer other);
        Code                disconnectFrom(const ProcessorPointer other);

        virtual bool        equalTo(const ProcessorPointer other) const;

        ProcessorType       type() const;
        bool                typeIs(ProcessorType value) const;
        bool                opened() const;
        bool                closed() const;
        bool                discardType(MediaType type) const;
        std::string         metaData() const;

    protected:

        uint64_t            inputDataCount()            const;
        uint64_t            outputDataCount()           const;
        uint64_t            discardedDataCount()        const;
        uint64_t            processingIterationCount()  const;

        void                increaseInputDataCount();
        void                increaseOutputDataCount();
        void                increaseDiscardedDataCount();
        void                increaseProcessingIterationCount();

    protected:

        StreamMap           _stream_map;

    private:

        const int64_t       _uid;
        ProcessorType       _type;
        bool                _opened;
        bool                _close_on_disconnect; //TODO убрать? 10.01
        int64_t             _discard_types;
        StreamVector        _streams;
        std::string         _meta_data; //TODO not used 10.01
        uint64_t            _input_data_count;
        uint64_t            _output_data_count;
        uint64_t            _discarded_data_count;
        uint64_t            _processing_iterations_count;

    };

} // namespace fpp
