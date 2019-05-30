#include <iostream>
#include "YMediaChain.h"

using namespace std;

int main()
{

//    av_log_set_level(AV_LOG_VERBOSE);
//    av_log_set_level(AV_LOG_DEBUG);
    cout << "Program started..." << endl;



//    std::string mrl_src = "rtsp://admin:Admin2019@192.168.10.12";
    std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
//    std::string mrl_src = "rtsp://192.168.0.14:8080/h264";



    std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/8sfh-322a-063b-ahuk";
//    std::string mrl_dst = "remuxed.flv";



    YMediaSource source(mrl_src);
    source.audio_parameters.setIgnore(true);
    YMediaSource silence("aevalsrc=10", YMediaPreset::Silence);
    YMediaDestination destination(mrl_dst, YMediaPreset::YouTube);



    YMediaChain chain(&source, &destination);
    chain.setContingencyAudioSource(&silence);



    chain.start();
    while (chain.active()) {}

    cout << "Program finished." << endl;
    return 0;

}
