#include "EncoderContext.hpp"

namespace fpp {

    EncoderContext::EncoderContext(const IOParams params) :
        CodecContext(params, CodecType::Encoder)
    {
        setName("EncoderContext");
    }

    EncoderContext::~EncoderContext() {
        // TODO
    }

    Code EncoderContext::encode(Frame input_frame, Packet& output_packet) {
        try_to(output_packet.init());
        output_packet.setType(params.out->type());
        auto debug_value = this;
        auto op = opened();
        log_trace("Frame for encoding: " << input_frame);
//        std::cout << "Frame for encoding: " << input_frame << std::endl;
//        std::cout << "EncoderContext: " << this->toString() << std::endl;
//        if (AVPixelFormat(input_frame.raw().format) == AVPixelFormat::AV_PIX_FMT_YUV420P) {
//            utils::SaveAvFrame(&input_frame.raw());
//        }
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
//        return_if_not(_stream->inited(), Code::NOT_INITED);
        utils::parameters_to_context(params.out, _codec_context);
        //findme
//        utils::parameters_to_avcodecpar(_stream->parameters, _stream->codecParameters());
//        if (avcodec_parameters_to_context(_codec_context, _stream->codecParameters()) < 0) {
//            log_error("avcodec_parameters_to_context failed");
//            return Code::ERR;
//        }
        return Code::OK;
    }

} // namespace fpp
