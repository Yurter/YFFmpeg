#include "Decoder.hpp"

namespace fpp {

    Decoder::Decoder(Stream *stream) :
        Codec(stream, CodecType::Decoder) {
        setName("Decoder");
    }

    Code Decoder::processInputData(Packet& input_data) {
        if (!input_data.empty()) {
            if (avcodec_send_packet(_codec_context, &input_data.raw()) != 0) {
//                log_error("Could not send packet");
                log_error("Could not send packet: " << input_data);
                log_error("Codeec_context's width: " << _codec_context->width
                          << ", height: " << _codec_context->height);
                return Code::ERR;
            }
            av_packet_unref(&input_data.raw());
        }
        Frame output_data;
        int ret = avcodec_receive_frame(_codec_context, output_data.raw());
        switch (ret) {
        case 0:
            output_data.setType(input_data.type());
//            log_warning("Sending: " << output_data);
            return sendOutputData(output_data);
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
