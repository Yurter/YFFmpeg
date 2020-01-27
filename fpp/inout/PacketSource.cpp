#include "PacketSource.hpp"

namespace fpp {

    PacketSource::PacketSource()
    {
//        setName("PacketSource"); //TODO заменить все остальные сеттеры
        setName(typeid(*this).name()); // TODO переместить внутрь метода name() ? длинные имена
        setType(ProcessorType::Input);
        try_throw(setPreFunction(std::bind(&PacketSource::readPacket, this)));
    }

    PacketSource::~PacketSource() {
        //TODO
    }

    Code PacketSource::readPacket() { //TODO refactoring
        Packet packet;
        try_to(readInputData(packet));
        try_to(storeInputData(packet));
        increaseInputDataCount();
        return Code::OK;
    }

} // namespace fpp
