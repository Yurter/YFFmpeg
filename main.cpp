#include <iostream>
#include "control/YFFmpeg.hpp"

using namespace std;

int main()
{

    cout << "[main] Program started..." << endl;

//    set_log_level(YLogLevel::Debug);
//    set_log_level(YLogLevel::Quiet);
//    set_ffmpeg_log_level(YLogLevel::Quiet);

    /* Запись rtsp с камеры в flv/YouTube */
//     std::string mrl_src = "camera_video.avi";
    std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
//    std::string mrl_src = "rtsp://192.168.0.14:8080/h264";

//    std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/2qqv-7ttx-xhk0-az48";
    std::string mrl_dst = "remuxed.flv";
//    std::string mrl_dst = "remuxed.avi";
//    std::string mrl_dst = "camera_sound.aac";

    auto source = new YSource(mrl_src);
    auto destination = new YDestination(mrl_dst, YMediaPreset::YouTube);
//    auto destination = new YDestination(mrl_dst);

    YFFmpeg ffmpeg;
    ffmpeg.addElement(source);
    ffmpeg.addElement(destination);
//    ffmpeg.setOptions(YOption::COPY_AUDIO);
//    ffmpeg.setOptions(YOption::COPY_VIDEO);

//    ffmpeg.setRoute(source, 0, destination, 0);
//    ffmpeg.setRoute(source, 1, destination, 1);

    if (auto ret = ffmpeg.start(); ret != YCode::OK) {
        cout << "[main] YFFmpeg start failed: " << ret << " - " << utils::code_to_string(ret) << endl;
    }
    ffmpeg.join();

    cout << "[main] Program finished." << endl;
    return 0;

}
