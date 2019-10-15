#include <iostream>
#include "fpp.hpp"

using namespace std;
using namespace fpp;

int main() {

    cout << "[main] Program started..." << endl;

    try {
//        set_log_level(LogLevel::Debug);
    //    set_log_level(LogLevel::Quiet);
//        set_ffmpeg_log_level(LogLevel::Quiet);
//        set_ffmpeg_log_level(LogLevel::Debug);

        /* Запись rtsp с камеры в flv/YouTube */
    //     std::string mrl_src = "camera_video.avi";
    //    std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
    //    std::string mrl_src = "rtsp://admin:Admin2019@192.168.10.12";
        std::string mrl_src = "camera_video_10.flv";
//        std::string mrl_src = "big_buck_bunny.mp4";
//        std::string mrl_src = "rtsp://admin:admin@192.168.10.3";

    //    std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/2qqv-7ttx-xhk0-az48";
    //    std::string mrl_dst = "remuxed.flv";
    //    std::string mrl_dst = "remuxed.avi";
//        std::string mrl_dst = "filtered_video.mp4";
         std::string mrl_dst = "result_video.flv";
//        std::string mrl_dst = "filtered_video.avi";
    //    std::string mrl_dst = "camera_sound.aac";

        auto source = new Source(mrl_src);
//        auto sink = new Sink(mrl_dst);
        auto sink = new Sink(mrl_dst, MediaPreset::Timelapse);

        Pipeline pipeline;
        pipeline.addElement(source);
        pipeline.addElement(sink);
    //    ffmpeg.setOptions(Option::COPY_AUDIO);
    //    ffmpeg.setOptions(Option::COPY_VIDEO);

    //    ffmpeg.setRoute(source, 0, destination, 0);
    //    ffmpeg.setRoute(source, 1, destination, 1);

        if (auto ret = pipeline.start(); ret != Code::OK) {
            cout << "[main] Pipeline start failed: " << ret << " - " << utils::code_to_string(ret) << endl;
        }
        pipeline.join();

        cout << "[main] Program finished." << endl;

    }
    catch (std::exception e) {
        cout << "[main] exception: " << e.what() << endl;
    }
    catch (...) {
        cout << "[main] exception: unknown" << endl;
    }

    return 0;

}
