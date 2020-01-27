#include "DecoderContext.hpp"
#include <fpp/core/Logger.hpp>
#include <fpp/core/Utils.hpp>
#include <fpp/core/FFmpegException.hpp>

namespace fpp {

    DecoderContext::DecoderContext(const IOParams params)
        : CodecContext(params) {
        setName("DecCtx");
    }

    DecoderContext::~DecoderContext() {
        // TODO EMPTY_DESTRUCTOR ? context->close() ?
        flush(nullptr);
    }

    FrameList DecoderContext::decode(Packet input_packet) {
        if (avcodec_send_packet(context().get(), &input_packet.raw()) != 0) {
            throw FFmpegException { "avcodec_send_packet failed" };
        }
        FrameList decoded_frames;
        while (true) {
            Frame output_frame;
            const int ret = avcodec_receive_frame(context().get(), &output_frame.raw());
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return decoded_frames;
            if (ret < 0) {
                throw FFmpegException { "av_buffersink_get_frame failed" };
            }
            output_frame.setType(params.in->type());
            decoded_frames.push_back(output_frame);
        }
    }

    Code DecoderContext::flush(Object* data) { //TODO
        return Code::OK;
        log_error("FLUSH");
        while (avcodec_send_packet(context().get(), nullptr) != 0) {
            Frame output_frame;
            while (avcodec_receive_frame(context().get(), &output_frame.raw()) != 0) {
                log_error("Flushed success");
            }
        }
        log_error("FLUSH finished");
        return Code::OK;
        if (int ret = avcodec_send_packet(context().get(), nullptr); ret != 0) {
            char errstr[1024];
            ffmpeg::av_strerror(ret, errstr, 1024); //TODO сделать методом уилит 23.01
            log_error("Failed to flush decoder: " << errstr);
            return Code::FFMPEG_ERROR;
        }
        Frame output_frame;
        int ret = avcodec_receive_frame(context().get(), &output_frame.raw());
        switch (ret) {
        case 0:
            output_frame.setType(MediaType::Video); //TODO
            if (output_frame.empty()) {
                log_error("Sending empty frame: " << output_frame);
            }
            log_error("Flushed success");
            return Code::OK;
//            return sendOutputData(output_data);
        case AVERROR(EAGAIN):
            return Code::AGAIN;
        case AVERROR_EOF:
            return Code::END_OF_FILE;
        case AVERROR(EINVAL):
            return Code::INVALID_INPUT;
        default:
            return Code::ERR;
        }
    }

    const ffmpeg::AVCodec* DecoderContext::codec() {
        return params.in->codec();
    }

    SharedParameters DecoderContext::parameters() {
        return params.in;
    }

} // namespace fpp
