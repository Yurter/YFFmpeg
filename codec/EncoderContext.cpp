#include "EncoderContext.hpp"

namespace fpp {

    EncoderContext::EncoderContext(const IOParams params) :
        CodecContext(params, CodecType::Encoder)
    {
        setName("EncoderContext");
    }

    EncoderContext::~EncoderContext() {
        // TODO
        flush(nullptr);
    }

    Code EncoderContext::encode(Frame input_frame, Packet& output_packet) {
        try_to(output_packet.init());
        output_packet.setType(params.out->type());
        output_packet.setTimeBase(params.in->timeBase());
        log_trace("Frame for encoding: " << input_frame);
        int ret;
        if ((ret = avcodec_send_frame(_codec_context, &input_frame.raw())) != 0) {
            log_error(input_frame);
            log_error("Pxl_fmt: " << input_frame.raw().format << " cc: " << _codec_context->pix_fmt);
            log_error("Could not send frame " << ret);
            return Code::ERR;
        }
//        input_frame.free(); //memfix
        if ((ret = avcodec_receive_packet(_codec_context, &output_packet.raw())) != 0) {
//            log_warning("avcodec_receive_packet failed");
            return Code::AGAIN;
        }
        output_packet.setStreamIndex(params.out->streamIndex());
        output_packet.setStreamUid(params.out->streamUid());
        return Code::OK;
    }

    Code EncoderContext::initParams() {
        utils::parameters_to_context(params.out, _codec_context);
        return Code::OK;
    }

    Code EncoderContext::flush(Object *data) {
        return Code::OK;
        log_error("FLUSH");
        while (avcodec_send_frame(_codec_context, nullptr) != 0) {
            Packet output_packet;
            try_to(output_packet.init());
            while (avcodec_receive_packet(_codec_context, &output_packet.raw()) != 0) {
                log_error("Flushed success");
            }
        }
        log_error("FLUSH finished");
        return Code::OK;
        Packet output_packet;
        try_to(output_packet.init());
        output_packet.setType(params.out->type());
        auto debug_value = this;
        auto op = opened();
//        log_trace("Frame for encoding: " << input_frame);
        int ret;
        if ((ret = avcodec_send_frame(_codec_context, nullptr)) != 0) {
            log_error("failed to flush encoder " << ret);
            return Code::ERR;
        }
//        input_frame.free(); //memfix
        if ((ret = avcodec_receive_packet(_codec_context, &output_packet.raw())) != 0) {
//            log_warning("avcodec_receive_packet failed");
            return Code::AGAIN;
        }
        log_error("Flushed success");
        output_packet.setStreamIndex(params.out->streamIndex());
        output_packet.setStreamUid(params.out->streamUid());
        return Code::OK;
    }

} // namespace fpp
