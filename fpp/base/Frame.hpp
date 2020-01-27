#pragma once
#include <fpp/base/Data.hpp>
#include <list>

extern "C" {
    #include <libavutil/frame.h>
}

namespace fpp {

    class Frame : public Data<AVFrame> {

    public:

        Frame();
        Frame(const Frame& other);
        Frame(const AVFrame& frame, MediaType type);
        virtual ~Frame() override;

        Frame& operator=(const Frame& other);

        int64_t             pts() const;
        void                setPts(int64_t value);

        bool                keyFrame()      const;

        virtual uint64_t    size()      const override;
        virtual std::string toString()  const override;

    private:

        void                copy(const Frame& other);
        void                copy(const AVFrame& other, MediaType type);

    };

    using FrameList = std::list<Frame>;

} // namespace fpp
