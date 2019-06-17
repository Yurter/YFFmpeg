#include "YEncoder.h"

YEncoder::YEncoder()
{
    YAbstractCodec::setName("YEncoder");
}

YCode YEncoder::processInputData(YFrame &input_data)
{
    YPacket output_data;
    output_data.init();
    output_data.setType(input_data.type());
    AVCodecContext *codec_context = nullptr;
    if (input_data.isVideo()) { codec_context = _video_codec_context; }
    if (input_data.isAudio()) { codec_context = _audio_codec_context; }
    if (codec_context == nullptr) {
        std::cerr << "[YEncoder] codec_context is null" << std::endl;
        return YCode::ERR;
    }
    int ret;
    if ((ret = avcodec_send_frame(codec_context, input_data.raw())) != 0) {
        std::cerr << "[YEncoder] Could not send frame " << ret << std::endl;
        return YCode::ERR;
    }
    if ((ret = avcodec_receive_packet(codec_context, &output_data.raw())) != 0) {
        return YCode::AGAIN;
    }
    input_data.free(); //TODO
    sendOutputData(output_data);
    return YCode::OK;
}
