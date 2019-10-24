#pragma once
#include "FormatContext.hpp"

namespace fpp {

    class OutputFormatContext : public FormatContext {

    public:

        OutputFormatContext();
        virtual ~OutputFormatContext() override;

        virtual Code createContext()    override;
        virtual Code openContext()      override;

    };

} // namespace fpp
