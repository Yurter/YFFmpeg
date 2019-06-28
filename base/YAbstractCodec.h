#pragma once

#include "YDataProcessor.h"
#include "../base/YStream.h"
#include "../base/YFrame.h"
#include "../base/YPacket.h"

template <class inType, class outType>
class YAbstractCodec : public YDataProcessor<inType,outType>
{

public:

    YAbstractCodec(YStream* stream) :
        _stream(stream),
        _codec_context(nullptr)
    {
        setName("YAbstractCodec");
    }

    virtual ~YAbstractCodec()
    {
        avcodec_close(_codec_context);
    }

    YCode init()
    {
        AVCodec* codec = findCodec();
        return_if(not_inited_ptr(codec), YCode::INVALID_INPUT);
        {
            _codec_context = avcodec_alloc_context3(codec);
            setName(name() + " " + codec->name);
        }
        if (not_inited_ptr(_codec_context)) {
            log_error("Failed to alloc context");
            return YCode::ERR;
        }
        utils::parameters_to_avcodecpar(_stream->parameters, _stream->codecParameters());
        if (avcodec_parameters_to_context(_codec_context, _stream->codecParameters()) < 0) {
            log_error("avcodec_parameters_to_context failed");
            return YCode::ERR;
        }
        utils::parameters_to_context(_stream->parameters, _codec_context);
        if (avcodec_open2(_codec_context, codec, nullptr) < 0) {
            log_error("Cannot open codec");
            return YCode::ERR;
        }
        { /* Crutch */ //TODO
            if (_stream->isAudio()) {
                _codec_context->channel_layout = static_cast<uint64_t>(
                    av_get_default_channel_layout(_codec_context->channels)
                );
            }
        }
        {
            setInited(true);
            return YCode::OK;
        }
    }

    std::string toString() const override final // ?
    {
        std::string str = "TODO";
        return str;
    }

    AVCodecContext* codecContext()
    {
        return _codec_context;
    }

protected:

    virtual AVCodec*    findCodec() = 0;

protected:

    // Media
    YStream*            _stream;

    // FFmpeg
    AVCodecContext*     _codec_context;

};
