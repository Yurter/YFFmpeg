#include <iostream>
#include "control/YFFmpeg.hpp"

using namespace std;

int main()
{

    cout << "[main] Program started..." << endl;

    set_log_level(YLogLevel::Debug);
    set_ffmpeg_log_level(YLogLevel::Quiet);

    /* Запись rtsp с камеры в flv/YouTube */
    std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
//    std::string mrl_src = "rtsp://192.168.0.14:8080/h264";

//    std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/ytub-8t5w-asjj-avyf";
//    std::string mrl_dst = "remuxed.flv";
    std::string mrl_dst = "camera_sound.aac";

    auto source = new YSource(mrl_src);
//    auto destination = new YDestination(mrl_dst, YMediaPreset::YouTube);
    auto destination = new YDestination(mrl_dst);

    YFFmpeg ffmpeg;
    ffmpeg.addElement(source);
    ffmpeg.addElement(destination);
//    ffmpeg.setOptions(COPY_VIDEO);

//    ffmpeg.setRoute({{source, 0}, {destination, 0}});
//    ffmpeg.setRoute({{source, 1}, {destination, 1}});

//    ffmpeg.setRoute({{source, 1}, {destination, 0}});

    ffmpeg.setRoute(source, 1, destination, 0);
    auto ret = ffmpeg.start();
    if (ret != YCode::OK) {
        cout << "[main] YFFmpeg start failed: " << ret << " - " << utils::code_to_string(ret) << endl;
    }
    ffmpeg.join();

    cout << "[main] Program finished." << endl;
    return 0;

}
