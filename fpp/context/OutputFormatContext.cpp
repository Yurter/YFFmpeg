#include "OutputFormatContext.hpp"
#include <fpp/core/Utils.hpp>

extern "C" {
    #include <libavformat/avformat.h>
}

namespace fpp {

    OutputFormatContext::OutputFormatContext(const std::string& mrl, Preset preset)
        : FormatContext { mrl, preset }
        , _output_format { nullptr } {
        setName("OutFmtCtx");
        /*try_throw*/(init());
    }

    OutputFormatContext::~OutputFormatContext() {
        try_throw(close());
    }

    void OutputFormatContext::init() { //TODO refactoring 14.01
//        return_if(inited(), Code::OK);
        /*try_to*/(createContext());
        switch (preset()) {
        case Preset::Auto: {
            /*try_to*/(guessOutputFromat());
            break;
        }
        case Preset::Event: {
            StreamVector stream_list;
            /* Video */
            auto video_params = std::make_shared<VideoParameters>();
            video_params->setEncoder(AVCodecID::AV_CODEC_ID_H264);
            video_params->setGopSize(2);
            video_params->setTimeBase(DEFAULT_TIME_BASE);
            stream_list.push_back(createStream(video_params));
            setStreams(stream_list);
            break;
        }
        default:
            log_error("Invalid preset");
            break;
        }
        log_debug("Created");
        setInited(true);
//        return Code::OK;
    }

    Code OutputFormatContext::write(Packet packet, ReadWriteMode write_mode) {
        stampPacket(packet);
        if (write_mode == ReadWriteMode::Instant) {
            const int ret = av_write_frame(context().get(), &packet.raw());
            if (ret < 0) {
                throw FFmpegException { "av_write_frame failed", ret };
            }
        }
        else if (write_mode == ReadWriteMode::Interleaved) {
            const int ret = av_interleaved_write_frame(context().get(), &packet.raw());
            if (ret < 0) {
                throw FFmpegException { "av_interleaved_write_frame failed", ret };
            }
        }
        return Code::OK;
    }

    Code OutputFormatContext::flush() {
        if (closed()) {
            throw std::logic_error("Flush failed: OutputFormatContext closed");
        }
        if (av_write_frame(context().get(), nullptr) < 0) {
            return Code::FFMPEG_ERROR;
        }
        return Code::OK;
    }

    Code OutputFormatContext::createContext() {
        auto format_short_name = utils::guess_format_short_name(mediaResourceLocator());
        AVFormatContext* fmt_ctx = nullptr;
        if (avformat_alloc_output_context2(&fmt_ctx, nullptr, format_short_name, mediaResourceLocator().c_str()) < 0) {
            log_error("Failed to alloc output context.");
            return Code::ERR;
        }
        setFormatContext(SharedAVFormatContext {
                             fmt_ctx
                             , [](auto* fmt_ctx) { avformat_free_context(fmt_ctx); }
                         }
        ); //TODO переделать в кастомный аллакатор 17.01
        return Code::OK;
    }

    Code OutputFormatContext::openContext() {
        return_if(opened(), Code::OK);
        return_if_not(inited(), Code::NOT_INITED);
        if (!(context()->flags & AVFMT_NOFILE)) {
            if (avio_open(&context()->pb, mediaResourceLocator().c_str(), AVIO_FLAG_WRITE) < 0) {
                log_error("Could not open output: " << mediaResourceLocator());
                return Code::INVALID_INPUT;
            }
        }
        if (avformat_write_header(context().get(), nullptr) < 0) {
            log_error("avformat_write_header failed: " << mediaResourceLocator());
            return Code::ERR;
        }
        return Code::OK;
    }

    Code OutputFormatContext::closeContext() {
        return_if(closed(), Code::OK);
        if (av_write_trailer(context().get()) != 0) {
            log_error("Failed to write the stream trailer to an output media file");
            return Code::ERR;
        }
        avio_close(context()->pb);
        return Code::OK;
    }

    SharedStream OutputFormatContext::createStream(SharedParameters params) {
        auto avstream = avformat_new_stream(context().get(), params->codec()); //TODO все где get() перенести в методы форматконтекста 17.01
        params->setStreamIndex(avstream->index);
        return std::make_shared<Stream>(avstream, params);
    }

    Code OutputFormatContext::guessOutputFromat() {
        auto out_fmt = ::av_guess_format(nullptr, mediaResourceLocator().c_str(), nullptr);
        setOutputFormat(out_fmt);
        return Code::OK;
    }

    Code OutputFormatContext::parseOutputFormat() {
        return Code::NOT_IMPLEMENTED;
//        return_if_not(inited_ptr(_output_format), Code::NOT_INITED);
//        {
//            //TODO:                                   ↓ mp3 AVOutputFormat дает видеокодек PNG ↓
//            auto& video_codec_id = _output_format->video_codec;
//            if (inited_codec_id(video_codec_id) && _output_format->video_codec != AV_CODEC_ID_PNG) {
//                try_to(createStream(utils::default_video_parameters(video_codec_id)));
//            }
//        }
//        {
//            auto& audio_codec_id = _output_format->audio_codec;
//            if (inited_codec_id(audio_codec_id)) {
//                try_to(createStream(utils::default_audio_parameters(audio_codec_id)));
//            }
//        }
        //        return Code::OK;
    }

    AVOutputFormat* OutputFormatContext::outputFormat() {
        return _output_format;
    }

    void OutputFormatContext::setOutputFormat(AVOutputFormat* out_fmt) {
        _output_format = out_fmt;
    }

} // namespace fpp
