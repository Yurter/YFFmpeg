#pragma once
#include "../base/Codec.hpp"

namespace fpp {

    class Encoder : public Codec<Frame,Packet> {

    public:

        Encoder(Stream* stream);
        virtual ~Encoder() override = default;

    private:

        virtual Code processInputData(Frame& input_data) override;

    };

    using EncoderList = std::list<Encoder*>;

} // namespace fpp
