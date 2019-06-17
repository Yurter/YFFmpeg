#include "YEncoder.h"

YEncoder::YEncoder()
{
    setName("YEncoder");
}

YCode YEncoder::processInputData(YFrame& input_data)
{
    YPacket output_data;
    output_data.init();
    output_data.setType(_type);
    int ret;
    if ((ret = avcodec_send_frame(_codec_context, input_data.raw())) != 0) {
        log_error("Could not send frame " << ret);
        return YCode::ERR;
    }
    if ((ret = avcodec_receive_packet(_codec_context, &output_data.raw())) != 0) {
        return YCode::AGAIN;
    }
    input_data.free(); //TODO
    return sendOutputData(output_data);
}
