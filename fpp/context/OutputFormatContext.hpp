#pragma once
#include <fpp/base/FormatContext.hpp>

namespace fpp {

    class OutputFormatContext : public FormatContext {

    public:

        OutputFormatContext(const std::string& mrl, Preset preset = Preset::Auto);
        virtual ~OutputFormatContext() override;

        Code                write(Packet packet, ReadWriteMode write_mode = ReadWriteMode::Instant);
        Code                flush();

    private:

        virtual void        init()          override;
        virtual Code        createContext() override;
        virtual Code        openContext()   override;
        virtual Code        closeContext()  override;

        SharedStream        createStream(SharedParameters params);
        Code                guessOutputFromat();
        Code                parseOutputFormat();

    private:

        AVOutputFormat* outputFormat();
        void                    setOutputFormat(AVOutputFormat* out_fmt);

    private:

        AVOutputFormat* _output_format;

    };

} // namespace fpp
