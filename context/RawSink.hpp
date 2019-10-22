#pragma once
#include "base/Context.hpp"

namespace fpp {

    class RawSink : public Context {

    public:

        RawSink(const std::string& mrl, IOType preset = IOType::Raw);
        virtual ~RawSink() override;

//        virtual Code        init() override;
//        virtual Code        open() override;
//        virtual Code        close() override;
//        virtual std::string toString() const override;

//        AVOutputFormat*     outputFrormat() const; //TODO убрать?

//        Code                guessOutputFromat();

    private:

//        virtual Code        createContext() override;
//        virtual Code        openContext() override;

////        Code                write();

//        Code                writePacket(Packet& packet);

//        virtual Code        processInputData(Packet& input_data) override;

//        Code                parseOutputFormat();

    private:

//        //FFmpeg
//        AVOutputFormat*     _output_format;

    };

    using RawSinkList = std::list<RawSink*>;

} // namespace fpp
