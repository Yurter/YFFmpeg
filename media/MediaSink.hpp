#pragma once
#include "PacketSink.hpp"
#include "base/IOContext.hpp"

namespace fpp {

    class MediaSink : public PacketSink, public IOContext {

    public:

        MediaSink(const std::string mrl, IOType preset = IOType::Auto);
        virtual ~MediaSink() override;

        virtual Code init() override;
        virtual Code open() override;
        virtual Code close() override;
        virtual std::string toString() const override;

    private:

        virtual Code        createContext() override;
        virtual Code        openContext() override;
        virtual Code        processInputData(Packet input_data) override;
        virtual Code        writeOutputData(Packet output_data) override;
        virtual Code        onStop() override;

        Code                guessOutputFromat();

//    private:

        Code                parseOutputFormat();

    private:

        //FFmpeg
        AVOutputFormat*     _output_format;

    };

    using MediaSinkkList = std::list<MediaSink*>;

} // namespace fpp
