#include "EncoderContext.hpp"

namespace fpp {

    EncoderContext::EncoderContext(Stream* stream) :
        CodecContext(stream, CodecType::Encoder)
    {
        setName("EncoderContext");
    }

    EncoderContext::~EncoderContext() {
        // TODO
    }

    Code EncoderContext::encode(Frame input_frame, Packet& output_packet) {
        try_to(output_packet.init());
        output_packet.setType(_stream->type());
        int ret;
//        utils::SaveAvFrame(input_frame.raw());
        if ((ret = avcodec_send_frame(_codec_context, input_frame.raw())) != 0) {
            log_error(input_frame);
            log_error("Pxl_fmt: " << input_frame.raw()->format << " cc: " << _codec_context->pix_fmt);
            log_error("Could not send frame " << ret);
            return Code::ERR;
        }
        input_frame.free(); //memfix
        if ((ret = avcodec_receive_packet(_codec_context, &output_packet.raw())) != 0) {
//            log_warning("avcodec_receive_packet failed");
            return Code::AGAIN;
        }
        output_packet.setStreamIndex(_stream->parameters->streamIndex());
        output_packet.setStreamUid(_stream->uid());
        return Code::OK;
    }

    Code EncoderContext::initParams() {
        utils::parameters_to_context(_stream->parameters, _codec_context);
        utils::parameters_to_avcodecpar(_stream->parameters, _stream->codecParameters());
        if (avcodec_parameters_to_context(_codec_context, _stream->codecParameters()) < 0) {
            log_error("avcodec_parameters_to_context failed");
            return Code::ERR;
        }
        return Code::OK;
    }

} // namespace fpp
