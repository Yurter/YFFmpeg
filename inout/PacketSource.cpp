#include "PacketSource.hpp"

namespace fpp {

    PacketSource::PacketSource()
    {
        setName("PacketSource");
        try_throw(setPreFunction(std::bind(&PacketSource::readPacket, this)));
    }

    PacketSource::~PacketSource() {
        //TODO
    }

    Code PacketSource::readPacket() {
        Packet packet;
        Code ret = readInputData(packet);
        if (utils::exit_code(ret)
                && (ret != Code::END_OF_FILE)) {
            return ret;
        }
        try_to(storeInputData(packet));
        return Code::OK;
    }

} // namespace fpp
