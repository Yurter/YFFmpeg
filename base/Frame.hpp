#pragma once
#include "Data.hpp"

namespace fpp {

    class Frame : public Data<AVFrame> {

    public:

        Frame();
        Frame(const Frame& other);
//        Frame(const Frame&& other);
//        Frame(AVFrame* frame); //TODO релиазовать
        virtual ~Frame() override;

        Frame& operator=(const Frame&& other);

        int64_t             pts() const;

        void                setPts(int64_t value);

        virtual uint64_t    size() const override;
        virtual std::string toString() const override;

    };

} // namespace fpp
