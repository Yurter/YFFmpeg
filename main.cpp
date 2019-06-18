#include <iostream>
#include "control/YMediaChain.h"

using namespace std;

int main()
{

    cout << "Program started..." << endl;

    set_log_level(YLogLevel::Debug);

    /* Запись rtsp с камеры в flv/YouTube */
    std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
//    std::string mrl_src = "rtsp://192.168.0.14:8080/h264";

    std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/ytub-8t5w-asjj-avyf";
//    std::string mrl_dst = "remuxed.flv";

    YMediaChain chain;
    chain.addElement(new YSource(mrl_src));
    chain.addElement(new YDestination(mrl_dst, YMediaPreset::YouTube));
    chain.setOptions(COPY_VIDEO);

    chain.start();
    chain.join();

    cout << "Program finished." << endl;
    return 0;

}
