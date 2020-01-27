#pragma once
#include <fpp/base/FormatContext.hpp>

namespace fpp {

    /* ? */
    enum class SeekPrecision {
        Forward,
        Backward,
        Precisely,
        Any,
    };

    class InputFormatContext : public FormatContext {

    public:

        InputFormatContext(const std::string& mrl, Preset preset = Preset::Auto);
        virtual ~InputFormatContext() override;


        Code                seek(int64_t stream_index, int64_t timestamp, SeekPrecision seek_precision = SeekPrecision::Forward);
        Code                read(Packet& packet, ReadWriteMode read_mode = ReadWriteMode::Instant);


        static std::string  silence(int64_t sample_rate) {
            return "anullsrc=r=" + std::to_string(sample_rate) + ":cl=mono";
        }

        static std::string  sine(int64_t frequency, int64_t sample_rate) {
            return "sine=frequency=" + std::to_string(frequency)
                    + ":sample_rate=" + std::to_string(sample_rate);
        }

    private:

        virtual void        init()          override;
        virtual Code        createContext() override;
        virtual Code        openContext()   override;
        virtual Code        closeContext()  override;

        [[nodiscard]]
        StreamVector        parseFormatContext();
        Code                guessInputFromat();

        ffmpeg::AVInputFormat*  inputFormat();
        void                    setInputFormat(ffmpeg::AVInputFormat* in_fmt);

    private:

        ffmpeg::AVInputFormat*  _input_format;

    };

} // namespace fpp
