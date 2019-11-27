#include "Decoder.hpp"

namespace fpp {

    Decoder::Decoder(Stream* stream) :
        _decoder_context(stream)
    {
        setName("Decoder");
    }

    Code Decoder::init() {
        log_trace("Initialization.");
        try_to(_decoder_context.init());
        return Code::OK;
    }

    Code Decoder::open() {
        log_trace("Opening.");
        try_to(_decoder_context.open());
        setOpened(true);
        return Code::OK;
    }

    Code Decoder::close() {
        log_trace("Closing.");
        try_to(_decoder_context.close());
        setOpened(false);
        return Code::OK;
    }

    Code Decoder::processInputData(Packet input_data) {
        Frame decoded_frame;
        Code ret = _decoder_context.decode(input_data, decoded_frame);
        return_if(utils::exit_code(ret), ret);
        if (ret == Code::OK) {
            try_to(sendOutputData(decoded_frame));
        }
        return ret;
    }

    AVCodecContext* Decoder::decoderContext() {
        return _decoder_context.codecContext();
    }

} // namespace fpp
