#pragma once
#include "core/async/AsyncList.hpp"
#include "core/Thread.hpp"
#include "stream/Stream.hpp"
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
        Processor& operator=(const Processor& other)  = delete;
        virtual ~Processor() override;

        virtual Code        open();
        virtual Code        close();
        virtual Code        push(const Object* input_data) = 0; //TODO 1) убрать виртуальность метода и перенести на уровень выше
                                                                //     2) избавиться от каста объекта в пакет или фрейм - сделать тип шаблонным 13.01
                                                                //   => проблема - метод необходим в базовом классе, а тип даты опредеятся выше по уровню

        int64_t             uid()                       const;
        StreamVector        streams()                   const;
        StreamPointer       stream(int64_t index)       const;
        StreamPointer       bestStream(MediaType type)  const;

        Code                setStreams(StreamVector streams);
        void                setType(ProcessorType value);
        void                setOpened(bool opened);

        Code                connectTo(StreamId_t stream_id, ProcessorPointer other);
        Code                disconnectFrom(const ProcessorPointer other);

        virtual bool        equalTo(const ProcessorPointer other) const;

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
