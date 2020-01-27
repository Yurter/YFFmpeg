//#pragma once
//#include "inout/FrameSource.hpp"

//namespace fpp {

//    class CustomFrameSource : public FrameSource {

//    public:

//        CustomFrameSource(std::string source_name
//                           , StreamVector streams
//                           , std::function<Code(Packet&)> read_func
//                           , std::function<Code(Packet&)> process_func);
//        virtual ~CustomFrameSource() override;

//        virtual Code        init() override;
//        virtual Code        open() override;
//        virtual Code        close() override;
//        virtual std::string toString() const override;

//        StreamVector        streams();
//        Stream*             stream(int64_t index);
//        Stream*             stream(MediaType type);

//    private:

////        virtual Code        readInputData(Packet& input_data) override;
////        virtual Code        processInputData(Packet input_data) override;

//        std::string                     _source_name;
//        StreamVector                    _streams;
//        std::function<Code(Packet&)>    _read_func;
//        std::function<Code(Packet&)>    _process_func;

//    };

//} // namespace fpp
