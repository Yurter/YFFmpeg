#pragma once
#include "base/FormatContext.hpp"

namespace fpp {

    enum class SeekPrecision {
        Forward,
        Backward,
        Precisely,
        Any,
    };

    class InputFormatContext : public FormatContext {

    public:

        InputFormatContext(const std::string mrl, /*ProcessorPointer stream_context,*/ IOType preset = IOType::Auto);
        virtual ~InputFormatContext() override;

        virtual Code        init()           override;
        virtual std::string toString() const override;

        Code                seek(int64_t stream_index, int64_t timestamp, SeekPrecision seek_precision = SeekPrecision::Forward);

    private:

        virtual Code        createContext() override;
        virtual Code        openContext()   override;
        virtual Code        closeContext()  override;

        Code                guessInputFromat();

    private:

        AVInputFormat*      _input_format;

    };

} // namespace fpp
