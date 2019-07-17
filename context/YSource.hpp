#pragma once

#include "base/YContext.hpp"

class YSource : public YContext
{

public:

    YSource(const std::string& mrl, YMediaPreset preset = YMediaPreset::Auto);
    virtual ~YSource() override;

    YCode               init() override;
    YCode               open() override;
    YCode               close() override;
    std::string         toString() const override;
    operator            std::string() const; //TODO заменить метод на оператор

    AVInputFormat*      inputFormat() const;  //TODO убрать? IOFormat() ?

private:

    YCode               guessInputFromat();
    YCode               createContext() override;
    YCode               openContext() override;

    YCode               read();

    YCode               processInputData(YPacket& input_data) override;

    void                parseInputFormat(); //TODO : YContext::parseIOFormat ?

private:

    // FFmpeg
    AVInputFormat*  _input_format; //TODO убрать, обращатся через mediaFormatContext()? IOFormat() ?

};
