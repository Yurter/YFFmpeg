#include "Encoder.hpp"

namespace fpp {

    Encoder::Encoder(Stream* stream) :
        Codec(stream, CodecType::Encoder)
    {
        setName("Encoder");
    }

    Code Encoder::initParams() {
        utils::parameters_to_context(_stream->parameters, _codec_context);
        utils::parameters_to_avcodecpar(_stream->parameters, _stream->codecParameters());
        if (avcodec_parameters_to_context(_codec_context, _stream->codecParameters()) < 0) {
            log_error("avcodec_parameters_to_context failed");
            return Code::ERR;
        }
        return Code::OK;
    }

    Code Encoder::processInputData(Frame input_data) {
        Packet output_data;
        try_to(output_data.init());
        output_data.setType(_stream->type());
        int ret;
//        log_warning("Send frame: " << input_data);
//        log_warning(_codec_context->pix_fmt << ":" << input_data.raw()->format);
//        utils::SaveAvFrame(input_data.raw());
        if ((ret = avcodec_send_frame(_codec_context, input_data.raw())) != 0) {
            log_error(input_data);
//            utils::save_frame_as_jpeg(_codec_context, input_data.raw(), 0);
            log_error("Pxl_fmt: " << input_data.raw()->format << " cc: " << _codec_context->pix_fmt);
//            utils::SaveAvFrame(input_data.raw());
            log_error("Could not send frame " << ret);
            return Code::ERR;
        }
        input_data.free(); //memfix
        if ((ret = avcodec_receive_packet(_codec_context, &output_data.raw())) != 0) {
//            log_warning("avcodec_receive_packet failed");
            return Code::AGAIN;
        }
        output_data.setStreamIndex(_stream->parameters->streamIndex());
        output_data.setStreamUid(_stream->uid());
//        log_debug("Sending... " << output_data);
        return sendOutputData(output_data);
    }

} // namespace fpp
