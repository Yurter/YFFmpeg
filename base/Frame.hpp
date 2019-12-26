#pragma once
#include "Data.hpp"

namespace fpp {

    class Frame : public Data<AVFrame> {

    public:

        Frame();
        Frame(const Frame& other);
        Frame(const Frame&& other);
        Frame(const AVFrame& frame, MediaType type, AVRational time_base);
        virtual ~Frame() override;

        Frame& operator=(const Frame& other);
        Frame& operator=(const Frame&& other);

        int64_t             pts()       const;
        AVRational          timeBase()  const;

        void                setPts(int64_t value);
        void                setTimeBase(AVRational time_base);

        virtual uint64_t    size()      const override;
        virtual std::string toString()  const override;

    private:

        void                copyOther(const Frame& other);
        void                copyOther(const AVFrame& other, MediaType type, AVRational time_base);

    private:

        AVRational          _time_base;

    };

} // namespace fpp
