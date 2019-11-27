#include "fpp.hpp"

using namespace std;
using namespace fpp;

void start_debug_timeout(MediaSource* source, int delay) {
    std::thread([source,delay]() {
        int step_ms = delay * 10;
        for (auto i = 0; i < 100; i++) {
            utils::sleep_for_ms(step_ms);
            static_log_info("external_stop", "Progress " << i << "%");
        }
//        exit(0); // Temp
        Code ret = source->stop();
        static_log_info("external_stop", "Source stopped: " << ret << " - " << utils::code_to_string(ret));
    }).detach();
}

//void start_debug_timeout(MediaSource* source, int delay) {
//    std::thread([source,delay]() {
//        int step_sec = 2;
//        int delay_sec = delay;
//        for (auto i = 0; i < (delay_sec / step_sec); i++) {
//            utils::sleep_for_sec(step_sec);
//            int proc = int((float((i + 1) * step_sec) / delay_sec) * 100);
//            static_log_info("external_stop", "Progress " << proc << "%");
//        }
//        exit(0); // Temp
//        Code ret = source->stop();
//        static_log_info("external_stop", "Source stopped: " << ret << " - " << utils::code_to_string(ret));
//    }).detach();
//}

int main() {

    static_log_info("main", "Program started...");

    try {
        set_log_level(LogLevel::Debug);
//        set_log_level(LogLevel::Quiet);

//        set_ffmpeg_log_level(LogLevel::Quiet);
//        set_ffmpeg_log_level(LogLevel::Trace);

        avdevice_register_all();
//        {
//            Pipeline* pipeline = new Pipeline;
//            if (auto ret = pipeline->start(); ret != Code::OK) {
//                static_log_error("main", "Pipeline start failed: " << ret << " - " << utils::code_to_string(ret));
//            }

//            auto source = new MediaSource("video=HP Wide Vision FHD Camera"); /* IP Camera */
//            source->setCloseOnDisconnect(false);
//            pipeline->addElement(source);


//            auto sink_event = new MediaSink("group_video/eventUSB.flv", IOType::Event);
//            pipeline->addElement(sink_event);

//            auto sink_timelapse = new MediaSink("group_video/timelapseUSB.flv", IOType::Timelapse);
//            pipeline->addElement(sink_timelapse);

////            auto params = new VideoParameters;
////            params->setStreamIndex(0);
////            auto custom_sink = new CustomPacketSink {
////                        "test"
////                        , { new VideoStream(params) }
////                        , [](Packet& packet) {
////                            UNUSED(packet);
////                            return Code::OK;
////                        }
////                        , [](Packet& packet) {
////                            UNUSED(packet);
////                            return Code::OK;
////                        }
////            };
////            pipeline->addElement(custom_sink);
//        }
        //Webcam C170
        //USB2.0 PC CAMERA
        //rtsp://admin:admin@192.168.10.189:554/ch01.264
        //rtsp://admin:Admin2019@192.168.10.12:554
        //system("route add 192.168.1.188 mask 255.255.255.255 192.168.137.124");
        {
            Pipeline* pipeline = new Pipeline;
            if (auto ret = pipeline->start(); ret != Code::OK) {
                static_log_error("main", "Pipeline start failed: " << ret << " - " << utils::code_to_string(ret));
            }

            auto source = new MediaSource("rtsp://admin:admin@192.168.10.3:554");
//            auto source = new MediaSource("video=Webcam C170");
            source->setCloseOnDisconnect(false);
            pipeline->addElement(source);


            auto sink_event = new MediaSink("group_video/eventIP.flv", IOType::Event);
            pipeline->addElement(sink_event);

            auto sink_timelapse = new MediaSink("group_video/timelapseIP.flv", IOType::Timelapse);
            pipeline->addElement(sink_timelapse);

            auto params = new VideoParameters;
            params->setCodec("libx264", CodecType::Encoder);
            params->setStreamIndex(0);
            auto custom_sink = new CustomPacketSink {
                        "test"
                        , { new VideoStream(params) }
                        , [](Packet& packet) {
                            UNUSED(packet);
                            return Code::OK;
                        }
                        , [](Packet& packet) {
                            UNUSED(packet);
                            return Code::OK;
                        }
            };
            pipeline->addElement(custom_sink);
            utils::sleep_for_min(1);
//            delete sink_event;
//            delete sink_timelapse;
        }

        /* Запись rtsp с камеры в flv/YouTube */
//        std::string mrl_src = "camera_one_hour_realtime.flv";
//        std::string mrl_src = "camera_video_10.flv";
//        std::string mrl_src = "camera_video.flv";
//        std::string mrl_src = "big_buck_bunny.mp4";
//        std::string mrl_src = "Walking.mp4"; /* Не работает декодер! */
//        std::string mrl_src = "rtsp://admin:Admin2019@192.168.10.12"; //640x480
//        std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
//        avdevice_register_all();
//        std::string mrl_src = "video=HP Wide Vision FHD Camera";

//        std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/vtpz-spss-u4eq-4k0e";
    //    std::string mrl_dst = "remuxed.flv";
    //    std::string mrl_dst = "remuxed.avi";
//        std::string mrl_dst = "filtered_video.mp4";
//         std::string mrl_dst = "result_video.flv";
//        std::string mrl_dst = "filtered_video.avi";
    //    std::string mrl_dst = "camera_sound.aac";

//        auto source = new MediaSource(mrl_src); /* IP Camera */
//        source->setCloseOnDisconnect(false);


//        static_log_info("main", "MSVC: " << /*_MSC_VER*/ _MSC_FULL_VER);

//        Pipeline pipeline;
//        pipeline.addElement(source);

//        auto sink_event = new MediaSink("group_video/event.flv", IOType::Event);
//        pipeline.addElement(sink_event);

//        auto video_params = new VideoParameters;
////        video_params->setWidth(640);
////        video_params->setHeight(480);
////        video_params->setFrameRate({ 22, 1 });
//        auto video_stream = new VideoStream(video_params);
//        auto sink_opencv = new OpenCVSink("OpenCV processing", video_stream);
//        pipeline.addElement(sink_opencv);

//        auto sink_youtube = new MediaSink("group_video/youtube.flv", IOType::YouTube);
//        pipeline.addElement(sink_youtube);


//        auto sink_restream = new MediaSink("group_video/restream.flv");
//        pipeline.addElement(sink_restream);

//        start_debug_timeout(source, 60 /** 60*/ /* час */); /* Таймаут для RTSP */

//        if (auto ret = pipeline.start(); ret != Code::OK) {
//            static_log_error("main", "Pipeline start failed: " << ret << " - " << utils::code_to_string(ret));
//        }


//        auto sink_timelapse = new MediaSink("group_video/timelapse.flv", IOType::Timelapse);
//        pipeline.addElement(sink_timelapse);

//        auto params = new VideoParameters;
//        params->setStreamIndex(0);
////        params->setWidth(2000);
//        auto custom_sink = new CustomPacketSink {
//                    "test"
//                    , { new VideoStream(params) }
//                    , [](Packet& packet) {
////                        std::cout << "write_func: " << packet << std::endl;
//                        return Code::OK;
//                    }
//                    , [](Packet& packet) {
////                        std::cout << "process_func: " << packet << std::endl;
//                        return Code::OK;
//                    }
//        };
//        pipeline.addElement(custom_sink);

//        std::thread([&pipeline](){
//            for (int i = 0; i < 60; i++) {
//                auto sink_timelapse = new MediaSink("group_video/timelapse" + std::to_string(i) + ".flv", IOType::Timelapse);
//                pipeline.addElement(sink_timelapse);
//                utils::sleep_for_sec(60);
//                pipeline.remElement(sink_timelapse);
////                sink_timelapse->close();
//                delete sink_timelapse;
//            }
//        }).detach();

//        std::thread([&pipeline](){
//            for (int i = 0; i < 60; i++) {
//                auto sink_event = new MediaSink("group_video/event" + std::to_string(i) + ".flv", IOType::Event);
//                pipeline.addElement(sink_event);
//                utils::sleep_for_sec(10);
//                pipeline.remElement(sink_event);
//                delete sink_event;
//                int t = 0;
//            }
//        }).detach();

//        utils::sleep_for_min(60);


//        start_debug_timeout(source, 60 /** 60*/ /* час */); /* Таймаут для RTSP */

//        pipeline.join();

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
