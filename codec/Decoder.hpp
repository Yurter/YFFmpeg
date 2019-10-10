#pragma once
#include "../base/Codec.hpp"

namespace fpp {

    class Decoder : public Codec<Packet,Frame> {

    public:

        Decoder(Stream* stream);
        virtual ~Decoder() override = default;

    private:

        virtual Code processInputData(Packet& input_data) override;

    };

} // namespace fpp
