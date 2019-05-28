#include <iostream>
#include "YMediaChain.h"

using namespace std;

int main()
{

//    av_log_set_level(AV_LOG_VERBOSE);
//    av_log_set_level(AV_LOG_DEBUG);
    cout << "Program started..." << endl;



    std::string mrl_src = "rtsp://admin:Admin2019@192.168.10.12";
//    std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
//    std::string mrl_src = "source_01.mp3";
//    std::string mrl_src = "source.mp3";
//    std::string mrl_src = "source16.mp3";
//      std::string mrl_src = "source16.flv";
//    std::string mrl_src = "source.wav";
//    std::string mrl_src = "source.flv";
//    std::string mrl_src = "rtsp://192.168.0.14:8080/h264";



//    std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/8sfh-322a-063b-ahuk";
    std::string mrl_dst = "remuxed.flv";
//    std::string mrl_dst = "remuxed.aac";
//    std::string mrl_dst = "remuxed.mp3";



    YMediaSource source(mrl_src);
    YMediaSource silence("aevalsrc=0", YMediaPreset::Silence);
//    YMediaDestination destination(mrl_dst, YMediaPreset::YouTube);
    YMediaDestination destination(mrl_dst, YMediaPreset::Auto);



//    destination.audio_parameters.setSampleRate(48'000);
////    destination.audio_parameters.setSampleRate(44'100);
////    destination.audio_parameters.setSampleRate(16'000);
////    destination.audio_parameters.setSampleRate(11'025);
//    destination.audio_parameters.setSampleFormat(AV_SAMPLE_FMT_FLTP);
//    destination.audio_parameters.setBitrate(128 * 1024);
//    destination.audio_parameters.setChanelsLayout(AV_CH_LAYOUT_STEREO);
//    destination.audio_parameters.setChanels(2);



    YMediaChain chain(&source, &destination);

    chain.start();
    while (chain.active()) {}

    cout << "Program finished." << endl;
    return 0;

}
