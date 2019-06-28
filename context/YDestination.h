#pragma once

#include "base/YContext.h"

class YDestination : public YContext
{

public:

    YDestination(const std::string& mrl, YMediaPreset preset = YMediaPreset::Auto);
    virtual ~YDestination() override;

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

    void                parseOutputFormat();

private:
public: //TODO

	// General parameters
//    int64_t             _video_packet_index;
//    int64_t             _audio_packet_index;

    //FFmpeg
    AVOutputFormat*     _output_format;

};
