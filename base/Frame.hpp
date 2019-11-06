#pragma once
#include "Data.hpp"

namespace fpp {

    class Frame : public Data<AVFrame*> {

    public:

        Frame();
        Frame(const Frame& other);
        Frame(const Frame&& other);
        Frame(AVFrame* frame);
        virtual ~Frame() override;

        Frame& operator=(const Frame&& other);

        bool                alloc();
        void                free();

        uint64_t            size() const;

        bool                empty() const;
        virtual Code        init() override;
        virtual std::string toString() const override;

    };

} // namespace fpp
