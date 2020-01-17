#pragma once
#include "inout/PacketSource.hpp"
#include "context/InputFormatContext.hpp"

namespace fpp {

    class MediaSource : public PacketSource {

    public:

        MediaSource(const std::string& mrl, IOPreset preset = IOPreset::Auto);
        virtual ~MediaSource() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;
        virtual bool        equalTo(const SharedProcessor other) const override;

        const InputFormatContext* inputFormatContext() const;

    private:

        virtual Code        readInputData(Packet& input_data) override;
        virtual Code        processInputData(Packet input_data) override;
        virtual Code        onStop() override;

//        void                determineStampType(const Packet& packet); //TODO забрать метод у потока 17.01

    private:

        InputFormatContext  _input_format_context;

    };

} // namespace fpp
