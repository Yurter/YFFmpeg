#pragma once

#include "base/YAbstractMedia.h"

class YMediaSource : public YAbstractMedia
{

public:

    YMediaSource(const std::string &mrl, YMediaPreset preset = YMediaPreset::Auto);
    virtual ~YMediaSource() override;

    bool            open()  override;
    bool            close() override;

    AVInputFormat*  inputFormat() const;

private:

    bool            guessInputFromat();
    bool            openInput();

    YCode           processInputData(YPacket& input_data) override;

    void            parseInputFormat(); //TODO : YAbstractMedia::parseIOFormat
    void            analyzePacket(YPacket& packet);

private:

    // FFmpeg
    AVInputFormat*  _input_format;

};
