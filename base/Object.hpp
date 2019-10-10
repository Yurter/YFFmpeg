#pragma once
#include "ffmpeg.hpp"
#include <string>
#include <iostream>

namespace fpp {

    class Object {

    public:

        Object();
        virtual ~Object() = default;

        void                setName(std::string name);
        std::string         name() const;
        bool                is(std::string name) const;

        virtual Code        init();
        void                setInited(bool inited);
        bool                inited() const;

        virtual std::string toString()      const;
        operator            std::string()   const;

    private:

        std::string         _name;
        bool                _inited;

    };

} // namespace fpp

////TODO
//namespace fpp {
//    // FFmpeg++
//    // fpp
//    // AVPipeLine
//    // YPipeLine
//    // PipeLine
//}
