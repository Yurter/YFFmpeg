#include "Decoder.hpp"

namespace fpp {

    Decoder::Decoder(Stream* stream) :
        _decoder_context(stream)
    {
        setName("Decoder");
        try_throw(setStreams({ stream }));
    }

    Code Decoder::init() {
        log_debug("Initialization.");
        try_to(_decoder_context.init());
        setInited(true);
        return Code::OK;
    }

    Code Decoder::open() {
        log_debug("Opening.");
        try_to(_decoder_context.open());
        setOpened(true);
        return Code::OK;
    }

    Code Decoder::close() {
        log_debug("Closing.");
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

    bool Decoder::equalTo(const Processor * const other) const {
        return_error_if_not(inited(), "Can't compare untill not inited.", false);
        return_error_if_not(other->inited(), "Can't compare untill not inited.", false);

        auto other_decoder = dynamic_cast<const Decoder * const>(other);
        return_if(not_inited_ptr(other_decoder), false);

        return_if(this->stream(0)->uid() == other->stream(0)->uid(), true);
        return false;
    }

    AVCodecContext* Decoder::decoderContext() {
        return _decoder_context.codecContext();
    }

} // namespace fpp
