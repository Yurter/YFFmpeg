#pragma once

#include "Data.hpp"

namespace fpp {

    class Frame : public Data<AVFrame*> {

    public:

        Frame();
        Frame(AVFrame* frame);
        virtual ~Frame() override;

        bool                alloc();
        void                free();

        bool                empty() const;
        Code                init() override;
        virtual std::string toString() const override;

    };

} // namespace fpp
