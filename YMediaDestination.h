#pragma once

#include "YAbstractMedia.h"

class YMediaDestination : public YAbstractMedia
{

public:

    YMediaDestination(const std::string &mrl, YMediaPreset preset = YMediaPreset::Auto);		// mrl - media resource locator
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
    void stampPacket(AVPacket &packet);

private:

	// General parameters
    int64_t             _frame_index;

    //FFmpeg
    //

};
