#pragma once
#include "base/Context.hpp"
#include <atomic>

namespace fpp {

    class Source : public Context {

    public:

        Source(const std::string& mrl, MediaPreset preset = MediaPreset::Auto);
        virtual ~Source() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;
//        operator            std::string() const; //TODO заменить метод на оператор

        AVInputFormat*      inputFormat() const;  //TODO убрать? IOFormat() ?

    private:

        Code                guessInputFromat();
        virtual Code        createContext() override;
        virtual Code        openContext() override;

        Code                read();

        virtual Code        processInputData(Packet& input_data) override;

        Code                sendEofPacket();

        void                parseInputFormat(); //TODO : Context::parseIOFormat ?

    private:

        // FFmpeg
        AVInputFormat*      _input_format; //TODO убрать, обращатся через mediaFormatContext()? IOFormat() ?

        //TODO
//        std::atomic_bool    _eof_flag;

    };

    using SourceList = std::list<Source*>;

} // namespace fpp
