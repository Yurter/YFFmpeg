#pragma once

#include "base/YAbstractMedia.h"

class YDestination : public YAbstractMedia
{

public:

    YDestination(const std::string& mrl, YMediaPreset preset = YMediaPreset::Auto);
    virtual ~YDestination() override;

    YCode               open() override;
    YCode               close() override;

    AVOutputFormat*     outputFrormat() const;

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
