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

private:

    bool            guessInputFromat();
    bool            openInput();

    void            run();

    void            parseInputFormat(); //TODO : YAbstractMedia::parseIOFormat
    void            analyzePacket(YPacket& packet);

private:

    // FFmpeg
    AVInputFormat*  _input_format;

};
