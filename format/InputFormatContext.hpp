#pragma once
#include "base/FormatContext.hpp"

namespace fpp {

    class InputFormatContext : public FormatContext {

    public:

        InputFormatContext(const std::string mrl, Object* media_ptr, IOType preset = IOType::Auto);
        virtual ~InputFormatContext() override;

        virtual Code        init()           override;
        virtual std::string toString() const override;

    private:

        virtual Code        createContext() override;
        virtual Code        openContext()   override;
        virtual Code        closeContext()  override;

        Code                guessInputFromat();

    private:

        AVInputFormat*      _input_format;

    };

} // namespace fpp
