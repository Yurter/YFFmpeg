#include <iostream>
#include "YMediaChain.h"

using namespace std;

int main()
{
    av_log_set_level(AV_LOG_VERBOSE);
    cout << "Program started..." << endl;



//    std::string mrl_src = "rtsp://admin:Admin2019@192.168.10.12";
//    std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
//    std::string mrl_src = "source.mp3";
    std::string mrl_src = "source.wav";



//    std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/rks9-hkxw-60f9-fk2e";
//    std::string mrl_dst = "findme.flv";
    std::string mrl_dst = "remuxed.mp3";



    YMediaSource source(mrl_src);
//    YMediaDestination destination(mrl_dst, YMediaDestination::YMediaPreset::YouTube);
    YMediaDestination destination(mrl_dst, YMediaDestination::YMediaPreset::Auto);



    destination.setSampleRate(44'100);
    destination.setSampleFormat(AV_SAMPLE_FMT_FLTP);
    destination.setAudioBitrate(128 * 1024);
    destination.setAudioChanelsLayout(AV_CH_LAYOUT_STEREO);
    destination.setAudioChanels(2);



    YMediaChain chain(&source, &destination);

    chain.start();
    while (chain.active()) {}

    cout << "Program finished." << endl;
    return 0;
}
