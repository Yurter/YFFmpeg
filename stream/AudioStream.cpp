//#include "AudioStream.hpp"
//#include "core/utils.hpp"

//namespace fpp {

//    AudioStream::AudioStream(AudioParameters* param) :
//        AudioStream(nullptr, param)
//    {
//        EMPTY_CONSTRUCTOR
//    }

//    AudioStream::AudioStream(AVStream* stream, AudioParameters* param) :
//        Stream(stream, param)
//    {
//        setName("AudioStream");
//    }

//    Code AudioStream::init() //TODO
//    {
//        try_to(Stream::init());
////        auto video_parameers = dynamic_cast<AudioParameters*>(params);

////        return_if(not_inited_q(video_parameers->timeBase()),  Code::INVALID_INPUT);

//        setInited(true);
//        return Code::OK;
//    }

//    bool AudioStream::operator>(const Stream& other) const {
//        auto this_params = static_cast<AudioParameters*>(params);
//        auto other_params = static_cast<AudioParameters*>(other.params);
//        return this_params->bitrate() > other_params->bitrate();
//    }

//} // namespace fpp
