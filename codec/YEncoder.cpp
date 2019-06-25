#include "YEncoder.h"

YEncoder::YEncoder(YStream* stream) :
    YAbstractCodec(stream)
{
    setName("YEncoder");
}

YEncoder::~YEncoder()
{
    // TODO
}

AVCodec* YEncoder::findCodec() //TODO check
{
//    return avcodec_find_encoder(_stream->codecParameters()->codec_id);
    return avcodec_find_encoder(_stream->parameters.codecId());
}


YCode YEncoder::processInputData(YFrame& input_data) //TODO
{
    YPacket output_data;
    output_data.init();
    output_data.setType(_stream->type());
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
