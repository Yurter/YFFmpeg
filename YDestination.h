#pragma once

#include "base/YAbstractMedia.h"

class YMediaDestination : public YAbstractMedia
{

public:

    YMediaDestination(const std::string &mrl, YMediaPreset preset = YMediaPreset::Auto);
    virtual ~YMediaDestination() override;

    bool addStream(AVCodecContext *stream_codec_context);
    bool open() override;
    bool close() override;

    AVOutputFormat*     outputFrormat() const;

    bool                guessOutputFromat();

private:

    bool createOutput();
    bool openOutput();

    YCode write();

    YCode processInputData(YPacket& input_data) override;

    void parseOutputFormat();
    bool stampPacket(YPacket &packet);

private:
public: //TODO

	// General parameters
    int64_t             _video_packet_index;
    int64_t             _audio_packet_index;

    //FFmpeg
    AVOutputFormat*     _output_format;

};