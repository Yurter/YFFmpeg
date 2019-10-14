#include "Decoder.hpp"

namespace fpp {

    Decoder::Decoder(Stream *stream) :
        Codec(stream, CodecType::Decoder) {
        setName("Decoder");
    }

    Code Decoder::processInputData(Packet& input_data) {
        if (!input_data.empty()) {
            if (avcodec_send_packet(_codec_context, &input_data.raw()) != 0) {
                log_error("Could not send packet");
                return Code::ERR;
            }
        }
        Frame output_data;
        int ret = avcodec_receive_frame(_codec_context, output_data.raw());
        switch (ret) {
        case 0:
            output_data.setType(input_data.type());
    //        log_error("decoded: " << output_data.toString());
    //        output_data.setStreamIndex(input_data.streamIndex());
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
