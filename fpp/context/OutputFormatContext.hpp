#pragma once
#include <fpp/base/FormatContext.hpp>

namespace fpp {

    class OutputFormatContext : public FormatContext {

    public:

        OutputFormatContext(const std::string& mrl, Preset preset = Preset::Auto);
        virtual ~OutputFormatContext() override;

        SharedStream        createStream(SharedParameters params);

        Code                write(Packet packet, ReadWriteMode write_mode = ReadWriteMode::Instant);
        Code                flush();

    private:

        virtual void        init()          override;
        virtual void        createContext() override;
        virtual void        openContext()   override;
        virtual void        closeContext()  override;

        Code                guessOutputFromat();
        Code                parseOutputFormat();

    private:

        AVOutputFormat* outputFormat();
        void                    setOutputFormat(AVOutputFormat* out_fmt);

    private:

        AVOutputFormat* _output_format;

    };

} // namespace fpp
