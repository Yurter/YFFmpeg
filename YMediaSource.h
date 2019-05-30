#pragma once

#include "base/YAbstractMedia.h"

class YMediaSource : public YAbstractMedia
{

public:

    YMediaSource(const std::string &mrl, YMediaPreset preset = YMediaPreset::Auto);
    virtual ~YMediaSource();

    bool open();
    bool close();

    bool readPacket(AVPacket &packet);

    AVInputFormat*      inputFormat() const;

private:

    bool openInput();
    void run();

private:

    // General parameters

    // FFmpeg
    AVInputFormat*      _input_format;
};
