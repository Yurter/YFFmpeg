#include "Decoder.hpp"

namespace fpp {

    Decoder::Decoder(const IOParams params) :
        _decoder_context(params)
    {
        setName("Decoder");
    }

    Decoder::~Decoder() {
        join();
    }

    Code Decoder::init() {
        log_debug("Initialization");
        try_to(_decoder_context.init());
        setInited(true);
        return Code::OK;
    }

    Code Decoder::open() {
        log_debug("Opening");
        try_to(_decoder_context.open());
        setOpened(true);
        return Code::OK;
    }

    Code Decoder::close() {
        log_debug("Closing");
        try_to(_decoder_context.close());
        setOpened(false);
        return Code::OK;
    }

    Code Decoder::processInputData(Packet input_data) {
        Frame decoded_frame;
        Code ret = _decoder_context.decode(input_data, decoded_frame);
        return_if(utils::exit_code(ret), ret);
        if (ret == Code::OK) {
            log_debug(decoded_frame);
            try_to(sendOutputData(decoded_frame));
        }
        return ret;
    }

    bool Decoder::equalTo(const ProcessorPointer other) const {
        auto other_decoder = dynamic_cast<const Decoder * const>(other.get());
        return_if(not_inited_ptr(other_decoder), false);

        return_if(this->decoderContext()->params.in->streamUid()
                  != other_decoder->decoderContext()->params.in->streamUid(), false);

        return true;
    }

    const DecoderContext* Decoder::decoderContext() const {
        return &_decoder_context;
    }

    Code Decoder::onStop() {
        log_debug("onStop");
        stopWait();
        return Code::OK;
    }

} // namespace fpp
