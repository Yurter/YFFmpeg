#pragma once

#include "base/YAbstractMedia.h"
#include "../codec/YDecoder.h"

class YSource : public YAbstractMedia
{

public:

    YSource(const std::string& mrl, YMediaPreset preset = YMediaPreset::Auto);
    virtual ~YSource() override;

    bool            open()  override;
    bool            close() override;

    AVInputFormat*  inputFormat() const;

private:

    bool            guessInputFromat();
    bool            openInput();

    YCode           read();

    YCode           processInputData(YPacket& input_data) override;

    void            parseInputFormat(); //TODO : YAbstractMedia::parseIOFormat

private:

    // FFmpeg
    AVInputFormat*  _input_format;

};
