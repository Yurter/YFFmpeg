#pragma once

#include "YData.hpp"

class YFrame : public YData<AVFrame*>
{

public:

    YFrame();
    YFrame(AVFrame* frame);
    virtual ~YFrame() override;

    bool                alloc();
    void                free();

    bool                empty() const;
    YCode               init() override;
    std::string         toString() const override;

private:

    // General
    //

};
