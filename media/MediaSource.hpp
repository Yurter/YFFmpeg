#pragma once
#include "inout/PacketSource.hpp"
#include "format/InputFormatContext.hpp"

namespace fpp {

    class MediaSource : public PacketSource {

    public:

        MediaSource(const std::string mrl, IOType preset = IOType::Auto);
        virtual ~MediaSource() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

        InputFormatContext& inputFormatContext();

    private:

        virtual Code        readInputData(Packet& input_data) override;
        virtual Code        processInputData(Packet input_data) override;
        virtual Code        onStop() override;

        Code                sendEofPacket();

    private:

        InputFormatContext  _input_format_context;

    };

    using MediaSourceList = std::list<MediaSource*>;

} // namespace fpp
