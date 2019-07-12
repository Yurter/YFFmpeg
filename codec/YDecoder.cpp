#include "YDecoder.h"

YDecoder::YDecoder(YStream *stream) :
    YAbstractCodec(stream)
{
    setName("YDecoder");
}

AVCodec* YDecoder::findCodec()
{
    return avcodec_find_decoder(_stream->codecParameters()->codec_id);
}

YCode YDecoder::processInputData(YPacket& input_data)
{
    if (!input_data.empty()) {
        if (avcodec_send_packet(_codec_context, &input_data.raw()) != 0) {
            log_error("Could not send packet");
            return YCode::ERR;
        }
    }
    YFrame output_data;
    int ret = avcodec_receive_frame(_codec_context, output_data.raw());
    switch (ret) {
    case 0:
        output_data.setType(input_data.type());
        output_data.setStreamIndex(input_data.streamIndex());
        return sendOutputData(output_data);
    case AVERROR(EAGAIN):
        return YCode::AGAIN;
    case AVERROR_EOF:
        return YCode::END_OF_FILE;
    case AVERROR(EINVAL):
        return YCode::INVALID_INPUT;
    default:
        return YCode::ERR;
    }
}
