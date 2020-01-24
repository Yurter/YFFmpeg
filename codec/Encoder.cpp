#include "Encoder.hpp"

namespace fpp {

    Encoder::Encoder(const IOParams params)
        : _encoder_context(params) {
        setName("Encoder");
    }

    Encoder::~Encoder() {
        try_throw(stop());
    }

    Code Encoder::init() {
        return_if(inited(), Code::INVALID_CALL_ORDER);
        log_debug("Initialization");
        try_to(_encoder_context.init());
        setInited(true);
        return Code::OK;
    }

    Code Encoder::open() {
        return_if(opened(), Code::INVALID_CALL_ORDER);
        log_debug("Opening");
//        try_to(_encoder_context.open());
        setOpened(true);
        return Code::OK;
    }

    Code Encoder::close() {
        return_if(closed(), Code::INVALID_CALL_ORDER);
        log_debug("Closing");
//        try_to(_encoder_context.close());
        setOpened(false);
        return Code::OK;
    }

    Code Encoder::processInputData(Frame input_data) {
        Packet encoded_packet;
        Code ret = _encoder_context.encode(input_data, encoded_packet);
        return_if(utils::exit_code(ret), ret);
        if (ret == Code::OK) {
            encoded_packet.setDts(encoded_packet.pts()); //TODO костыль, разобраться, почему смещение во времени (0, -45)
//            if (encoded_packet.isAudio()) log_warning("encoded audio: " << encoded_packet << " : " << _encoder_context.codecContext()->time_base);
            try_to(sendOutputData(encoded_packet));
        }
        return ret;
    }

    bool Encoder::equalTo(const SharedProcessor other) const {
        auto other_encoder = dynamic_cast<const Encoder * const>(other.get());
        return_if(not_inited_ptr(other_encoder), false);

        return_if(this->encoderContext()->params.in->streamUid()
                  != other_encoder->encoderContext()->params.in->streamUid(), false);

        return true;
    }

    const EncoderContext* Encoder::encoderContext() const {
        return &_encoder_context;
    }

    void Encoder::onStop() {
        log_debug("onStop");
        _encoder_context.flush(nullptr);
        stopWait();
    }

} // namespace fpp
