#pragma once
#include "FormatContext.hpp"

namespace fpp {

    class InputFormatContext : public FormatContext {

    public:

        InputFormatContext();
        virtual ~InputFormatContext() override;

        virtual Code createContext()    override;
        virtual Code openContext()      override;

    };

} // namespace fpp
