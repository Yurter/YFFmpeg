#pragma once

#include "base/YAbstractMedia.h"

class YMediaSource : public YAbstractMedia
{

public:

    YMediaSource(const std::string &mrl, YMediaPreset preset = YMediaPreset::Auto);
    virtual ~YMediaSource();

    bool            open();
    bool            close();

    bool            readPacket(YPacket &packet);

    AVInputFormat*  inputFormat() const;

    bool            guessInputFromat();

private:

    bool            openInput();
    void            run();

    void            analyzePacket(YPacket& packet);

private:

    // General
    //

    // FFmpeg
    AVInputFormat*  _input_format;
};
