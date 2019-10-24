#pragma once
#include "FormatContext.hpp"

namespace fpp {

    class InputFormatContext : public FormatContext {

    public:

        InputFormatContext(const std::string mrl, IOType preset = IOType::Auto);
        virtual ~InputFormatContext() override;

        virtual Code        init()          override;

    private:

        virtual Code        createContext() override;
        virtual Code        openContext()   override;
        virtual Code        closeContext()  override;

        Code                guessInputFromat();

    private:

        AVInputFormat*      _input_format;

    };

} // namespace fpp
