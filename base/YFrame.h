#pragma once

#include "YData.h"

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

    int64_t             streamIndex() const;
    void                setStreamIndex(int64_t stream_index);

private:

    // General
    int64_t             _stream_index;

};
