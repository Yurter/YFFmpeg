#pragma once
#include <fpp/inout/PacketSource.hpp>

namespace fpp {

    class CustomPacketSource : public PacketSource {

    public:

        CustomPacketSource(const std::string& source_name
                           , StreamVector streams
                           , const ReadFunction read_func
                           , const ProcessFunction process_func = [](Packet&) { return Code::OK; });
        virtual ~CustomPacketSource() override;

        virtual void        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

    private:

        virtual Code        readInputData(Packet& input_data) override;
        virtual Code        processInputData(Packet input_data) override;

        const std::string       _source_name;
        const ReadFunction      _read_func;
        const ProcessFunction   _process_func;

    };

} // namespace fpp
