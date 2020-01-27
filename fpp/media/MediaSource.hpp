#pragma once
#include <fpp/context/InputFormatContext.hpp>
#include <fpp/inout/PacketSource.hpp>

namespace fpp {

    class MediaSource : public PacketSource {

    public:

        MediaSource(const std::string& mrl, Preset preset = Preset::Auto);
        virtual ~MediaSource() override;

        virtual void        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;
        virtual bool        equalTo(const SharedProcessor other) const override;

        const InputFormatContext* inputFormatContext() const;

    private:

        virtual Code        readInputData(Packet& input_data) override;
        virtual Code        processInputData(Packet input_data) override;
        void                determineStampType(const Packet& packet);

    private:

        InputFormatContext  _input_format_context;

    };

} // namespace fpp
