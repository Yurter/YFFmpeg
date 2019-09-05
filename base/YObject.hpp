#pragma once
#include "ffmpeg.hpp"
#include <string>
#include <iostream>

class YObject {

public:

    YObject();
    virtual ~YObject() = default;

    void                setName(std::string name);
    std::string         name() const;
    bool                is(std::string name) const;

    virtual YCode       init();
    void                setInited(bool inited);
    bool                inited() const;

    virtual std::string toString()      const;
    operator            std::string()   const;

private:

    std::string         _name;
    bool                _inited;

};

//TODO
namespace fpp {
    // FFmpeg++
    // fpp
}
