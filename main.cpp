#include "fpp.hpp"

using namespace std;
using namespace fpp;

int main() {

    static_log_info("main", "Program started...");

    try {
//        set_log_level(LogLevel::Debug);
    //    set_log_level(LogLevel::Quiet);

        set_ffmpeg_log_level(LogLevel::Quiet);
//        set_ffmpeg_log_level(LogLevel::Debug);

        /* Запись rtsp с камеры в flv/YouTube */
//         std::string mrl_src = "camera_one_hour_realtime.flv";
//        std::string mrl_src = "camera_video_10.flv";
        std::string mrl_src = "camera_video.flv";
//        std::string mrl_src = "big_buck_bunny.mp4";
//        std::string mrl_src = "Walking.mp4";
//        std::string mrl_src = "rtsp://admin:Admin2019@192.168.10.12";
//        std::string mrl_src = "rtsp://admin:admin@192.168.10.3";

//        std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/vtpz-spss-u4eq-4k0e";
    //    std::string mrl_dst = "remuxed.flv";
    //    std::string mrl_dst = "remuxed.avi";
//        std::string mrl_dst = "filtered_video.mp4";
         std::string mrl_dst = "result_video.flv";
//        std::string mrl_dst = "filtered_video.avi";
    //    std::string mrl_dst = "camera_sound.aac";

        auto source = new Source(mrl_src);
//        auto sink = new Sink(mrl_dst);
        auto sink = new Sink(mrl_dst, MediaPreset::Timelapse);
        auto sink = new Sink("");
//        sink->ignoreType(MediaType::MEDIA_TYPE_AUDIO);
//        auto sink = new Sink(mrl_dst, MediaPreset::YouTube);

        std::thread([source]() {
            int step_sec = 2;
            int delay_sec = 64;
            for (auto i = 0; i < (delay_sec / step_sec); i++) {
                utils::sleep_for_sec(step_sec);
                int proc = int((float((i + 1) * step_sec) / delay_sec) * 100);
                static_log_info("external_stop", "Progress " << proc << "%");
            }
            Code ret = source->stop();
            static_log_info("external_stop", "Source stopped: " << ret << " - " << utils::code_to_string(ret));
        }).detach();

        Pipeline pipeline;
        pipeline.addElement(source);
        pipeline.addElement(sink);

        if (auto ret = pipeline.start(); ret != Code::OK) {
            static_log_error("main", "Pipeline start failed: " << ret << " - " << utils::code_to_string(ret));
        }
        pipeline.join();

        static_print_info("main", "Program finished.");

    }
    catch (std::exception e) {
         static_log_error("main", "exception: " << e.what());
    }
    catch (...) {
        static_log_error("main", "exception: unknown");
    }

    return 0;

}
