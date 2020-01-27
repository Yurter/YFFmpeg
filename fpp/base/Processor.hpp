#pragma once
#include <fpp/core/async/AsyncList.hpp>
#include <fpp/core/Thread.hpp>
#include <fpp/stream/Stream.hpp>
#include <fpp/base/Frame.hpp>
#include <map>
#include <variant>

namespace fpp {


    enum class ProcessorType { //TODO постараться избавиться от этого енама 15.01
        Unknown,
        Process,
        Output,
        Input,
    };

    // TODO бардак 17.01
    class Processor;
    using SharedProcessor = std::shared_ptr<Processor>;
    using ProcessorVector = std::vector<SharedProcessor>; //TODO не потокобезопасен

    using ProcessorList = AsyncList<SharedProcessor>;
    using StreamMap = std::map<uid_t,ProcessorList>;

    using InputData = std::variant<Packet,Frame>;

    class Processor : public Thread {

    public:

        Processor();
        Processor(const Processor& other)  = delete;
        Processor& operator=(const Processor& other)  = delete;
        virtual ~Processor() override;

        virtual Code        open();
        virtual Code        close();
        virtual Code        push(const InputData& input_data) = 0;

        int64_t             uid()                       const;
        StreamVector        streams()                   const;
        SharedStream        stream(int64_t index)       const;
        SharedStream        bestStream(MediaType type)  const;

        void                setStreams(StreamVector streams);
        void                setType(ProcessorType value);
        void                setOpened(bool opened);

        Code                connectTo(uid_t stream_id, SharedProcessor other);
        Code                disconnectFrom(const SharedProcessor other);

        virtual bool        equalTo(const SharedProcessor other) const;

        ProcessorType       type() const;
        bool                typeIs(ProcessorType value) const;
        bool                opened() const;
        bool                closed() const;

    protected:

        uint64_t            inputDataCount()        const;
        uint64_t            outputDataCount()       const;
        uint64_t            discardedDataCount()    const;
        uint64_t            processedDataCount()    const;

        void                increaseInputDataCount();
        void                increaseOutputDataCount();
        void                increaseDiscardedDataCount();
        void                increaseProcessedDataCount();

    protected:

        StreamMap           _stream_map;

    private:

        const int64_t       _uid;
        ProcessorType       _type;
        bool                _opened;
        StreamVector        _streams;
        uint64_t            _input_data_count;
        uint64_t            _output_data_count;
        uint64_t            _discarded_data_count;
        uint64_t            _processed_data_count;

    };

} // namespace fpp
