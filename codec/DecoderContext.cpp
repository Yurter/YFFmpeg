#include "DecoderContext.hpp"

namespace fpp {

    DecoderContext::DecoderContext(const IOParams params) :
        CodecContext(params, CodecType::Decoder) {
        setName("DecCtx");
    }

    DecoderContext::~DecoderContext() {
        // TODO EMPTY_DESTRUCTOR ? context->close() ?
        flush(nullptr);
    }

    Code DecoderContext::decode(Packet input_packet, Frame& decoded_frame) {
        if (int ret = avcodec_send_packet(_codec_context.get(), &input_packet.raw()); ret != 0) {
            char errstr[1024];
            ffmpeg::av_strerror(ret, errstr, 1024); //TODO сделать методом уилит 23.01
            log_error(" Could not send packet to decoder: " << errstr << ". Data: " << input_packet);
            log_error("DecoderContext: " << this->toString());
            return Code::FFMPEG_ERROR;
        }
        int ret = avcodec_receive_frame(_codec_context.get(), &decoded_frame.raw());
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

    Code DecoderContext::initParams() {
        utils::parameters_to_context(params.in, _codec_context.get());
        return Code::OK;
    }

    Code DecoderContext::flush(Object* data) { //TODO
        return Code::OK;
        log_error("FLUSH");
        while (avcodec_send_packet(_codec_context.get(), nullptr) != 0) {
            Frame output_frame;
            while (avcodec_receive_frame(_codec_context.get(), &output_frame.raw()) != 0) {
                log_error("Flushed success");
            }
        }
        log_error("FLUSH finished");
        return Code::OK;
        if (int ret = avcodec_send_packet(_codec_context.get(), nullptr); ret != 0) {
            char errstr[1024];
            ffmpeg::av_strerror(ret, errstr, 1024); //TODO сделать методом уилит 23.01
            log_error("Failed to flush decoder: " << errstr);
            return Code::FFMPEG_ERROR;
        }
        Frame output_frame;
        int ret = avcodec_receive_frame(_codec_context.get(), &output_frame.raw());
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

} // namespace fpp
