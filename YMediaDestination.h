#pragma once

#include "YAbstractMedia.h"

class YMediaDestination : public YAbstractMedia
{
public:

    YMediaDestination(const std::string &mrl, YMediaPreset preset = YMediaPreset::Auto);		// mrl - media resource locator
    virtual ~YMediaDestination();

    bool addStream(AVCodec *stream_codec);
    bool open();
    bool close();

    bool writePacket(AVPacket &&packet);

private:

	bool createOutputContext();
	bool copyInputContext(const AVFormatContext *input_format_context, bool audio_required, bool video_required);
	bool openOutputContext();

    void stampPacket(AVPacket &packet);

	bool muxVideoPacket();
	bool muxAudioPacket();

	void openVirtualAudioInput();
	void openVirtualVideoInput();

	AVPacket readVirtualAudioPacket();
	AVPacket readVirtualVideoPacket();

	void setVideoCodecContextOptions(AVCodecContext* avCodecContext);
	void setAudioCodecContextOptions(AVCodecContext* avCodecContext);

private:

	// General parameters
    int64_t     _frame_index;
    bool        _video_required;
    bool        _audio_required;

};
