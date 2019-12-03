#include "Encoder.hpp"

namespace fpp {

    Encoder::Encoder(Stream* stream) :
        _encoder_context(stream)
    {
        setName("Encoder");
        try_throw(setStreams({ stream }));
    }

    Code Encoder::init() {
        log_debug("Initialization.");
        try_to(_encoder_context.init());
        return Code::OK;
    }

    Code Encoder::open() {
        log_debug("Opening.");
        try_to(_encoder_context.open());
        setOpened(true);
        return Code::OK;
    }

    Code Encoder::close() {
        log_debug("Closing.");
        try_to(_encoder_context.close());
        setOpened(false);
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

    bool Encoder::equalTo(const Processor * const other) const {
        return_error_if_not(inited(), "Can't compare untill not inited.", false);
        return_error_if_not(other->inited(), "Can't compare untill not inited.", false);

        auto other_encoder = dynamic_cast<const Encoder * const>(other);
        return_if(not_inited_ptr(other_encoder), false);

        return_if(this->stream(0)->uid() == other->stream(0)->uid(), true);
        return false;
    }

    AVCodecContext* Encoder::encoderContext() {
        return _encoder_context.codecContext();
    }

} // namespace fpp
