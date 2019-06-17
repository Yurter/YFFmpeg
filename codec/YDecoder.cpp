#include "YDecoder.h"

YDecoder::YDecoder()
{
    YAbstractCodec::setName("YDecoder");
}

YCode YDecoder::processInputData(YPacket& input_data)
{
    if (!input_data.empty()) {
        if (avcodec_send_packet(_codec_context, &input_data.raw()) != 0) {
            std::cerr << "[YVideoDecoder] Could not send packet" << std::endl;
            return YCode::ERR;
        }
    }
    YFrame output_data;
    int ret = avcodec_receive_frame(_codec_context, output_data.raw());
    switch (ret) {
    case 0:
        output_data.setType(_type);
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
