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

    bool writePacket(AVPacket packet);

    AVOutputFormat*     outputFrormat() const;

    bool                guessOutputFromat();

private:

    bool createOutputContext();
	bool openOutputContext();

    void parseOutputFormat();


    bool run();

    void stampPacket(AVPacket &packet);

	void setVideoCodecContextOptions(AVCodecContext* avCodecContext);
	void setAudioCodecContextOptions(AVCodecContext* avCodecContext);

private:

	// General parameters
    int64_t             _frame_index;
    bool                _video_required;
    bool                _audio_required;

    //FFmpeg
    //

};
