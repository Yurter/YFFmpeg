#include "DecoderContext.hpp"
#include <fpp/core/Logger.hpp>
#include <fpp/core/Utils.hpp>

namespace fpp {

    DecoderContext::DecoderContext(const IOParams params)
        : CodecContext(params) {
        setName("DecCtx");
    }

    DecoderContext::~DecoderContext() {
        // TODO EMPTY_DESTRUCTOR ? context->close() ?
        flush(nullptr);
    }

    Code DecoderContext::decode(Packet input_packet, Frame& decoded_frame) {
        if (int ret = avcodec_send_packet(context().get(), &input_packet.raw()); ret != 0) {
            char errstr[1024];
            ffmpeg::av_strerror(ret, errstr, 1024); //TODO сделать методом уилит 23.01
            log_error(" Could not send packet to decoder: " << errstr << ". Data: " << input_packet);
            log_error("DecoderContext: " << this->toString());
            return Code::FFMPEG_ERROR;
        }
        int ret = avcodec_receive_frame(context().get(), &decoded_frame.raw());
        switch (ret) { //TODO убрать свич ?
        case 0:
            decoded_frame.setType(params.in->type());
            return Code::OK;
        case AVERROR(EAGAIN):
            return Code::AGAIN;
        case AVERROR_EOF:
            return Code::END_OF_FILE;
        case AVERROR(EINVAL):
            return Code::INVALID_INPUT;
        default:
            return Code::FFMPEG_ERROR;
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