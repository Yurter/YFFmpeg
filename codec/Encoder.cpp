#include "Encoder.hpp"

namespace fpp {

    Encoder::Encoder(Stream* stream) :
        Codec(stream, CodecType::Encoder)
    {
        setName("Encoder");
    }

    Code Encoder::processInputData(Frame& input_data) {
        Packet output_data;
        try_to(output_data.init());
        output_data.setType(_stream->type());
        int ret;
//        log_warning("Send frame: " << input_data);
        log_warning(_codec_context->pix_fmt << ":" << input_data.raw()->format);
        utils::SaveAvFrame(input_data.raw());
        if ((ret = avcodec_send_frame(_codec_context, input_data.raw())) != 0) {
            log_error(input_data);
//            utils::save_frame_as_jpeg(_codec_context, input_data.raw(), 0);
            log_error("Pxl_fmt: " << input_data.raw()->format << " cc: " << _codec_context->pix_fmt);
            utils::SaveAvFrame(input_data.raw());
            log_error("Could not send frame " << ret);
            return Code::ERR;
        }
        if ((ret = avcodec_receive_packet(_codec_context, &output_data.raw())) != 0) {
//            log_warning("avcodec_receive_packet failed");
            return Code::AGAIN;
        }
        input_data.free(); //TODO
        output_data.setStreamIndex(_stream->parameters->streamIndex());
        output_data.setStreamUid(_stream->uid());
        log_debug("Sending... " << output_data);
        return sendOutputData(output_data);
    }

} // namespace fpp
/*
[Encoder libx264]     Send frame: Video frame: 3110400 bytes, pts 575730, key_frame true, width 1920, height 1080
[Encoder libx264]     Send frame: Video frame: 3110400 bytes, pts 615330, key_frame false, width 1920, height 1080

[Encoder libx264]     Send frame: Video frame: 3110400 bytes, pts 82800, key_frame true, width 1920, height 1080
[Encoder libx264]     Send frame: Video frame: 3110400 bytes, pts 86400, key_frame false, width 1920, height 1080
*/
