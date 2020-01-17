#pragma once
#include "base/FormatContext.hpp"

namespace fpp {

    class InputFormatContext : public FormatContext {

    public:

        InputFormatContext(const std::string& mrl, IOPreset preset = IOPreset::Auto);
        virtual ~InputFormatContext() override;

        virtual Code        init()           override;
        virtual std::string toString() const override;

        Code                seek(int64_t stream_index, int64_t timestamp, SeekPrecision seek_precision = SeekPrecision::Forward);
        Code                read(Packet& packet, ReadWriteMode read_mode = ReadWriteMode::Instant);

    private:

        virtual Code        createContext() override;
        virtual Code        openContext()   override;
        virtual Code        closeContext()  override;

        [[nodiscard]]
        StreamVector        parseFormatContext();
        Code                guessInputFromat();

    private:

        AVInputFormat*      _input_format; //TODO зачем хранить копию из контекста? 14.01

    };

} // namespace fpp
