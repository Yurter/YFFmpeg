#pragma once

#include "base/YAbstractMedia.h"

class YMediaDestination : public YAbstractMedia
{

public:

    YMediaDestination(const std::string &mrl, YMediaPreset preset = YMediaPreset::Auto);
    virtual ~YMediaDestination();

    bool addStream(AVCodecContext *stream_codec_context);
    bool open();
    bool close();

    void writePacket(AVPacket packet);

    AVOutputFormat*     outputFrormat() const;

    bool                guessOutputFromat();

private:

    bool createOutputContext();
	bool openOutputContext();

    void run();

    void parseOutputFormat();
    bool stampPacket(AVPacket &packet);

private:
public: //TODO

	// General parameters
    int64_t             _video_packet_index;
    int64_t             _audio_packet_index;

    //FFmpeg
    AVOutputFormat*     _output_format;

};
