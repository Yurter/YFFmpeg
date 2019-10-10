#pragma once
#include "base/Context.hpp"

namespace fpp {

    class Sink : public Context {

    public:

        Sink(const std::string& mrl, MediaPreset preset = MediaPreset::Auto);
        virtual ~Sink() override;

        Code                init() override;
        Code                open() override;
        Code                close() override;
        std::string         toString() const override;

        AVOutputFormat*     outputFrormat() const; //TODO убрать?

        Code                guessOutputFromat();

    private:

        Code                createContext() override;
        Code                openContext() override;

        Code                write();

        Code                writePacket(Packet& packet);

        Code                processInputData(Packet& input_data) override;

        Code                parseOutputFormat();

    private:

        //FFmpeg
        AVOutputFormat*     _output_format;

    };

} // namespace fpp
