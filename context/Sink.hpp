#pragma once
#include "base/Context.hpp"

namespace fpp {

    class Sink : public Context {

    public:

        Sink(const std::string& mrl, IOType preset = IOType::Auto);
        virtual ~Sink() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual std::string toString() const override;

        AVOutputFormat*     outputFrormat() const; //TODO убрать?

        Code                guessOutputFromat();

    private:

        virtual Code        createContext() override;
        virtual Code        openContext() override;

//        Code                write();

        Code                writePacket(Packet& packet);

        virtual Code        processInputData(Packet& input_data) override;

        Code                parseOutputFormat();

    private:

        //FFmpeg
        AVOutputFormat*     _output_format;

    };

    using SinkList = std::list<Sink*>;

} // namespace fpp
