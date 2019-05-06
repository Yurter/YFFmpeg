#include <iostream>
#include "YMediaChain.h"

using namespace std;

int main()
{
    av_log_set_level(AV_LOG_VERBOSE);
    cout << "Program started..." << endl;

    //YMediaSource source("rtsp://192.168.0.13:8080/video/h264");
    YMediaSource source("F:\\Downloads_uTorrent\\Grinch.2018.MVO.BDRip.1400Mb_ExKinoRay_by_Twi7ter.avi");
    YMediaDestination destination("rtmp://a.rtmp.youtube.com/live2/jky1-6yv3-t2h3-dyx2", YMediaDestination::YMediaPreset::YouTube);

    YMediaChain chain(&source, &destination);
    chain.start();
    while (chain.active()) {}

    cout << "Program finished." << endl;
    return 0;
}
