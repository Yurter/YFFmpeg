#pragma once
#include <base/FormatContext.hpp>

namespace fpp {

    class OutputFormatContext : public FormatContext {

    public:

        OutputFormatContext(const std::string& mrl, Preset preset = Preset::Auto);
        virtual ~OutputFormatContext() override;

        Code                write(Packet packet, ReadWriteMode write_mode = ReadWriteMode::Instant);
        Code                flush();

    private:

        virtual Code        init()          override;
        virtual Code        createContext() override;
        virtual Code        openContext()   override;
        virtual Code        closeContext()  override;

        SharedStream        createStream(SharedParameters params);
        Code                guessOutputFromat();
        Code                parseOutputFormat();

    private:

        ffmpeg::AVOutputFormat* outputFormat();
        void                    setOutputFormat(ffmpeg::AVOutputFormat* out_fmt);

    private:

        ffmpeg::AVOutputFormat* _output_format;

    };

} // namespace fpp
