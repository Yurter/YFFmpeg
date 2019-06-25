#include <iostream>
#include "control/YFFmpeg.h"

using namespace std;

int main()
{

    cout << "Program started..." << endl;

    set_log_level(YLogLevel::Info);

    /* Запись rtsp с камеры в flv/YouTube */
    std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
//    std::string mrl_src = "rtsp://192.168.0.14:8080/h264";

    std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/ytub-8t5w-asjj-avyf";
//    std::string mrl_dst = "remuxed.flv";

    auto source = new YSource(mrl_src);
    auto destination = new YDestination(mrl_dst, YMediaPreset::YouTube);

    YFFmpeg ffmpeg;
    ffmpeg.addElement(source);
    ffmpeg.addElement(destination);
    ffmpeg.setOptions(COPY_VIDEO);

    ffmpeg.setRoute({source->stream(0), destination->stream(0)});
    ffmpeg.setRoute({source->stream(1), destination->stream(1)});

    ffmpeg.start();
    ffmpeg.join();

    cout << "Program finished." << endl;
    return 0;

}
