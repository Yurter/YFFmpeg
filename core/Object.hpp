#pragma once
#include "ffmpeg.hpp"
#include <string>
#include <iostream>
#include <sstream>

namespace fpp {

    class Object {

    public:

        Object();
        virtual ~Object() = default;

        /*constexpr*/ void      setName(const std::string& name);
        std::string         name()          const;
        bool                is(const std::string& name) const;

        virtual Code        init();
        void                setInited(bool inited);
        bool                inited() const;

        virtual std::string toString()      const;
        operator            std::string()   const;

        friend std::ostream& operator<<(std::ostream& os, const Object& object) {
            os << object.toString();
            return os;
        }

    private:

        /*const*/ std::string   _name;
        bool                _inited;

    };

} // namespace fpp
