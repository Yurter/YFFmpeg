#pragma once

#include "base/YContext.hpp"

class YDestination : public YContext
{

public:

    YDestination(const std::string& mrl, YMediaPreset preset = YMediaPreset::Auto);
    virtual ~YDestination() override;

    YCode               init() override;
    YCode               open() override;
    YCode               close() override;
    std::string         toString() const override;

    AVOutputFormat*     outputFrormat() const; //TODO убрать?

    YCode               guessOutputFromat();

private:

    YCode               createContext() override;
    YCode               openContext() override;

    YCode               write();

    YCode               writePacket(YPacket& packet);

    YCode               processInputData(YPacket& input_data) override;

    YCode               parseOutputFormat();

private:

    //FFmpeg
    AVOutputFormat*     _output_format;

};
