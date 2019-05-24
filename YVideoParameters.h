#pragma once

#include "YParameters.h"

class YVideoParameters : public YParameters
{

public:

    YVideoParameters();
    ~YVideoParameters();

    void            setWidth(int64_t width);
    void            setHeight(int64_t height);
    void            setAspectRatio(AVRational aspect_ratio);
    void            setFrameRate(float frame_rate);
    void            setPixelFormat(AVPixelFormat pixel_format);

    int64_t         width()         const;
    int64_t         height()        const;
    AVRational      aspectRatio()   const;
    float           frameRate()     const;
    AVPixelFormat   pixelFormat()   const;

private:

    int64_t         _width;
    int64_t         _height;
    AVRational      _aspect_ratio;
    float           _frame_rate;
    AVPixelFormat   _pixel_format;

};
