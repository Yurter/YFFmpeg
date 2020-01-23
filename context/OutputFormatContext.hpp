#pragma once
#include <base/FormatContext.hpp>

namespace fpp {

    class OutputFormatContext : public FormatContext {

    public:

        OutputFormatContext(const std::string& mrl, Preset preset = Preset::Auto);
        virtual ~OutputFormatContext() override;

        virtual Code        init()           override;

        Code                write(Packet packet, ReadWriteMode write_mode = ReadWriteMode::Instant);
        Code                flush();

    private:

        virtual Code        createContext() override;
        virtual Code        openContext()   override;
        virtual Code        closeContext()  override;

        SharedStream        createStream(SharedParameters params);
        Code                guessOutputFromat();
        Code                parseOutputFormat();

    private:

        ffmpeg::AVOutputFormat*     _output_format; //TODO убрать ? 14.01

    };

} // namespace fpp
