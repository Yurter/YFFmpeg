#pragma once
#include "../base/PacketProcessor.hpp"

namespace fpp {

    class PacketSource : public PacketProcessor {

    public:

        PacketSource();

        Code init();
        Code open();
        Code close();
        Code push();

        std::string toString() const;

    protected:

        Code processInputData(const Packet& input_data);


    };

} // namespace fpp
