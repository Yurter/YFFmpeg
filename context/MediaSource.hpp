#pragma once
#include "PacketSource.hpp"
#include "base/IOContext.hpp"

namespace fpp {

    class MediaSource : public PacketProducer, public IOContext {

    public:

        MediaSource(const std::string mrl, IOType preset = IOType::Auto);
        virtual ~MediaSource() override;

        virtual Code init() override;
        virtual Code open() override;
        virtual Code close() override;
        virtual std::string toString() const override;

    private:

        virtual Code        createContext() override;
        virtual Code        openContext() override;
        virtual Code        processInputData(Packet input_data) override;
        virtual Code        readInputData() override;
        virtual Code        onStop() override;

        Code                guessInputFromat();
        Code                sendEofPacket();

    private:

        AVInputFormat*      _input_format; //TODO убрать, обращатся через mediaFormatContext()? IOFormat() ?

    };

    using MediaSourceList = std::list<MediaSource*>;

} // namespace fpp
