#include "Encoder.hpp"

namespace fpp {

    Encoder::Encoder(Stream* stream) :
        _encoder_context(stream)
    {
        setName("Encoder");
    }

    Code Encoder::init() {
        try_to(_encoder_context.init());
        return Code::OK;
    }

    Code Encoder::open() {
        try_to(_encoder_context.open());
        return Code::OK;
    }

    Code Encoder::close() {
        try_to(_encoder_context.close());
        return Code::OK;
    }

    Code Encoder::processInputData(Frame input_data) {
        Packet encoded_packet;
        Code ret = _encoder_context.encode(input_data, encoded_packet);
        return_if(utils::exit_code(ret), ret);
        if (ret == Code::OK) {
            try_to(sendOutputData(encoded_packet));
        }
        return ret;
    }

    AVCodecContext* Encoder::encoderContext() {
        return _encoder_context.codecContext();
    }

} // namespace fpp
