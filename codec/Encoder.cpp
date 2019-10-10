#include "Encoder.hpp"

namespace fpp {

    Encoder::Encoder(Stream* stream) :
        Codec(stream)
    {
        setName("Encoder");
    }

    Code Encoder::processInputData(Frame& input_data) {
        Packet output_data;
        try_to(output_data.init());
        output_data.setType(_stream->type());
        int ret;
        if ((ret = avcodec_send_frame(_codec_context, input_data.raw())) != 0) {
            log_error(input_data.toString());
            log_error(_codec_context->codec->name);
            log_error("Could not send frame " << ret);
            return Code::ERR;
        }
        if ((ret = avcodec_receive_packet(_codec_context, &output_data.raw())) != 0) {
            return Code::AGAIN;
        }
        input_data.free(); //TODO
        output_data.setStreamIndex(_stream->parameters->streamIndex());
        output_data.setStreamUid(_stream->uid());
        return sendOutputData(output_data);
    }

} // namespace fpp
