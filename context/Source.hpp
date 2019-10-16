#pragma once
#include "base/Context.hpp"
#include <atomic>

namespace fpp {

    class Source : public Context {

    public:

        Source(const std::string& mrl, MediaPreset preset = MediaPreset::Auto);
        virtual ~Source() override;

        Code                init() override;
        Code                open() override;
        Code                close() override;
        std::string         toString() const override;
//        operator            std::string() const; //TODO заменить метод на оператор

        AVInputFormat*      inputFormat() const;  //TODO убрать? IOFormat() ?

    private:

        Code                guessInputFromat();
        Code                createContext() override;
        Code                openContext() override;

        Code                read();

        Code                processInputData(Packet& input_data) override;

        void                parseInputFormat(); //TODO : Context::parseIOFormat ?

    private:

        // FFmpeg
        AVInputFormat*      _input_format; //TODO убрать, обращатся через mediaFormatContext()? IOFormat() ?

        //TODO
//        std::atomic_bool    _eof_flag;

    };

    using SourceList = std::list<Source*>;

} // namespace fpp
