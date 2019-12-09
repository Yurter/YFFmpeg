#include "Encoder.hpp"

namespace fpp {

    Encoder::Encoder(const IOParams params) :
        _encoder_context(params)
    {
        setName("Encoder");
    }

    Code Encoder::init() {
        return_if(inited(), Code::INVALID_CALL_ORDER);
        log_debug("Initialization.");
        try_to(_encoder_context.init());
        setInited(true);
        return Code::OK;
    }

    Code Encoder::open() {
        return_if(opened(), Code::INVALID_CALL_ORDER);
        log_debug("Opening.");
        try_to(_encoder_context.open());
        setOpened(true);
        return Code::OK;
    }

    Code Encoder::close() {
        return_if(closed(), Code::INVALID_CALL_ORDER);
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

        return_if(_encoder_context.params.in->streamUid()
                  == other_encoder->encoderContext()->params.in->streamUid(), true);

        return false;
    }

    const EncoderContext* Encoder::encoderContext() const {
        return &_encoder_context;
    }

} // namespace fpp
