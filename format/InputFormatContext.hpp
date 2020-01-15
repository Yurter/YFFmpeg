#pragma once
#include "base/FormatContext.hpp"

namespace fpp {

    class InputFormatContext : public FormatContext {

    public:

        InputFormatContext(const std::string mrl, IOPreset preset = IOPreset::Auto);
        virtual ~InputFormatContext() override;

        virtual Code        init()           override;
        virtual std::string toString() const override;

        Code                seek(int64_t stream_index, int64_t timestamp, SeekPrecision seek_precision = SeekPrecision::Forward);
        Code                read(Packet& packet, ReadWriteMode read_mode = ReadWriteMode::Instant);

    private:

        virtual Code        createContext() override;
        virtual Code        openContext()   override;
        virtual Code        closeContext()  override;

        StreamVector        parseFormatContext();
        Code                guessInputFromat();
        void                initPacket(Packet& packet); //TODO убрать и перенести функционал в метод штампа потока 15.01

    private:

        AVInputFormat*      _input_format; //TODO зачем хранить копию из контекста? 14.01

    };

} // namespace fpp
