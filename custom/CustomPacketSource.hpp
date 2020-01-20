#pragma once
#include "inout/PacketSource.hpp"

namespace fpp {

    class CustomPacketSource : public PacketSource {

    public:

        CustomPacketSource(std::string source_name
                           , StreamVector streams
                           , ReadFunction read_func
                           , ProcessFunction process_func);
        virtual ~CustomPacketSource() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

    private:

        virtual Code        readInputData(Packet& input_data) override;
        virtual Code        processInputData(Packet input_data) override;

        std::string         _source_name;
        ReadFunction        _read_func;
        ProcessFunction     _process_func;

    };

} // namespace fpp
