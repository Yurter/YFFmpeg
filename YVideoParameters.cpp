#include "YVideoParameters.h"

YVideoParameters::YVideoParameters() :
    YParameters(),
    _width(-1),
    _height(-1),
    _aspect_ratio({-1,-1}),
    _frame_rate(-1.f),
    _pixel_format(AV_PIX_FMT_NONE)
{
    //
}

YVideoParameters::~YVideoParameters()
{
    //
}

void YVideoParameters::setWidth(int64_t width)
{
    _width = width - (width % 2);
}

void YVideoParameters::setHeight(int64_t height)
{
    _height = height - (height % 2);
}

void YVideoParameters::setAspectRatio(AVRational aspect_ratio)
{
    _aspect_ratio = aspect_ratio;
}

void YVideoParameters::setFrameRate(float frame_rate)
{
    _frame_rate = frame_rate;
}

void YVideoParameters::setPixelFormat(AVPixelFormat pixel_format)
{
    _pixel_format = pixel_format;
}

int64_t YVideoParameters::width() const
{
    return _width;
}

int64_t YVideoParameters::height() const
{
    return _height;
}

AVRational YVideoParameters::aspectRatio() const
{
    return _aspect_ratio;
}

float YVideoParameters::frameRate() const
{
    return _frame_rate;
}

AVPixelFormat YVideoParameters::pixelFormat() const
{
    return _pixel_format;
}
