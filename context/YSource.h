#pragma once

#include "base/YAbstractMedia.h"

class YSource : public YAbstractMedia
{

public:

    YSource(const std::string& mrl, YMediaPreset preset = YMediaPreset::Auto);
    virtual ~YSource() override;

    YCode               open() override;
    YCode               close() override;
    std::string         toString() const override;

    AVInputFormat*      inputFormat() const;  //TODO убрать?

private:

    YCode               guessInputFromat();
    YCode               createContext() override;
    YCode               openContext() override;

    YCode               read();

    YCode               processInputData(YPacket& input_data) override;

    void                parseInputFormat(); //TODO : YAbstractMedia::parseIOFormat ?

private:

    // FFmpeg
    AVInputFormat*  _input_format; //TODO убрать, обращатся через mediaFormatContext()

};
