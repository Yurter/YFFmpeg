#pragma once
#include "inout/PacketSource.hpp"
#include "format/InputFormatContext.hpp"

namespace fpp {

    class MediaSource : public PacketSource {

    public:

        MediaSource(const std::string mrl, IOType preset = IOType::Auto);
        virtual ~MediaSource() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

        virtual bool        equalTo(const ProcessorPointer other) const override;

        void                setStartPoint(int64_t value);
        void                setEndPoint(int64_t value);

        void        doNotSendEOF() { _doNotSendEOF = true; } //TODO костыль для конкатенации

        const InputFormatContext* inputFormatContext() const;

    private:

        virtual Code        readInputData(Packet& input_data) override;
        virtual Code        processInputData(Packet input_data) override;
        virtual Code        onStop() override;

        Code                sendEofPacket();

    private:

        InputFormatContext  _input_format_context;

        bool    _doNotSendEOF; //TODO

        int64_t             _start_time_point;
        int64_t             _end_time_point;

    };

} // namespace fpp
