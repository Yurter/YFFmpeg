#pragma once
#include "base/FormatContext.hpp"

namespace fpp {

    class OutputFormatContext : public FormatContext {

    public:

        OutputFormatContext(const std::string mrl, Object* media_ptr, IOType preset = IOType::Auto);
        virtual ~OutputFormatContext() override;

        virtual Code        init()           override;
        virtual std::string toString() const override;

    private:

        virtual Code        createContext() override;
        virtual Code        openContext()   override;
        virtual Code        closeContext()  override;

        Code                guessOutputFromat();
        Code                parseOutputFormat();

    private:

        AVOutputFormat*     _output_format;

    };

} // namespace fpp
