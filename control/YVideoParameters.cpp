#include "YVideoParameters.h"

YVideoParameters::YVideoParameters(YParameters parameters) :
    YParameters(parameters),
    _width(DEFAULT_INT),
    _height(DEFAULT_INT),
    _aspect_ratio(DEFAULT_RATIONAL),
    _frame_rate(DEFAULT_FLOAT),
    _pixel_format(DEFAULT_PIXEL_FORMAT)
{
    setName("YVideoParameters");
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

void YVideoParameters::setFrameRate(AVRational frame_rate)
{
    if (frame_rate.den == 0) {
        _frame_rate = -1.f;
    } else {
        _frame_rate = frame_rate.num / frame_rate.den;
    }
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

void YVideoParameters::toCodecpar(AVCodecParameters* codecpar)
{
    codecpar->width = int(_width);
    codecpar->height = int(_height);
    YParameters::toCodecpar(codecpar);
}

void YVideoParameters::softCopy(YVideoParameters &other_parametrs)
{
    if (not_inited_int(_width))             { _width = other_parametrs.width();                 }
    if (not_inited_int(_height))            { _height = other_parametrs.height();               }
    if (not_inited_q(_aspect_ratio))        { _aspect_ratio = other_parametrs.aspectRatio();    }
    if (not_inited_float(_frame_rate))      { _frame_rate = other_parametrs.frameRate();        }
    if (not_inited_pix_fmt(_pixel_format))  { _pixel_format = other_parametrs.pixelFormat();    }
    YParameters::softCopy(other_parametrs);
}

YVideoParameters& YVideoParameters::operator=(const YVideoParameters &rhs)
{
    _width          = rhs.width();
    _height         = rhs.height();
    _aspect_ratio   = rhs.aspectRatio();
    _frame_rate     = rhs.frameRate();
    _pixel_format   = rhs.pixelFormat();
    YParameters::operator=(rhs);
    return *this;
}
