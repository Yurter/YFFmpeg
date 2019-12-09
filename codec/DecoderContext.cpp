#include "DecoderContext.hpp"

namespace fpp {

    DecoderContext::DecoderContext(const IOParams params) :
        CodecContext(params, CodecType::Decoder)
    {
        setName("DecoderContext");EMPTY_CONSTRUCTOR
    }

    DecoderContext::~DecoderContext() {
        // TODO EMPTY_DESTRUCTOR ? context->close() ?
    }

    Code DecoderContext::decode(Packet input_packet, Frame& output_frame) {
        if (!input_packet.empty()) {
            if (int ret = avcodec_send_packet(_codec_context, &input_packet.raw()); ret != 0) {
                char errstr[1024];
                av_strerror(ret, errstr, 1024);
                log_error("Could not send packet to decoder: " << errstr << ". Data: " << input_packet);
                log_error("DecoderContext: " << this->toString());
//                log_error("Error: " << errstr << ". Data: " << input_packet);
//                log_error("Codeec_context's width: " << _codec_context->width
//                          << ", height: " << _codec_context->height);
                return Code::FFMPEG_ERROR;
            }
            av_packet_unref(&input_packet.raw());
        }
        int ret = avcodec_receive_frame(_codec_context, &output_frame.raw());
        switch (ret) {
        case 0:
            output_frame.setType(input_packet.type());
            if (output_frame.empty()) {
                log_error("Sending empty frame: " << output_frame);
            }
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

    Code DecoderContext::initParams() {
//        utils::parameters_to_avcodecpar(_params, _stream->codecParameters());
//        if (avcodec_parameters_to_context(_codec_context, _stream->codecParameters()) < 0) {
//            log_error("avcodec_parameters_to_context failed");
//            return Code::ERR;
//        } findme
        utils::parameters_to_context(params.in, _codec_context);
        return Code::OK;
    }

} // namespace fpp
