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

//    Concatenator concatenator("sausage.flv", {
//        { "input_0.flv", FROM_START, TO_END }
//        , { "input_1.flv", FROM_START, TO_END }
//    });
//    Concatenator concatenator("sausage.flv", {
//        { "input_0.flv", 5000, 8000 }
//        , { "input_1.flv", 6000, 9000 }
//    });

//    Concatenator concatenator("sausage.flv", {
//        { "timelapse.flv", FROM_START, TO_END }
//        , { "enent_0.flv", FROM_START, TO_END }
//        , { "enent_1.flv", FROM_START, TO_END }
//    });
//    set_log_level(LogLevel::Debug);

    #define TML(x) (x) / 60



//    Concatenator concatenator("test_2/sausage.flv", {
//        { "test_2/timelapse20191203-16.flv", FROM_START, TML(64136 - 15000) }
//        , { "test_2/event20191203-17-25-13-0.flv", FROM_START, TO_END }

////        , { "test_2/timelapse20191203-16.flv", TML(87383), TML(109401) }
////        , { "test_2/event20191203-17-25-58-1.flv", FROM_START, TO_END }
////        , { "test_2/timelapse20191203-16.flv", TML(117254), TML(212423) }
//        , { "test_2/timelapse20191203-16.flv", TML(87383), TML(212423 - 15000  - 15000) }

//        , { "test_2/event20191203-17-27-41-2.flv", FROM_START, TO_END }
//        , { "test_2/timelapse20191203-16.flv", TML(227317), TML(432523 - 15000) }
//        , { "test_2/event20191203-17-31-21-3.flv", FROM_START, TO_END }
//        , { "test_2/timelapse20191203-16.flv", TML(449576), TML(540719) }
//    });

    for (int i = 0; i < 100; ++i) {
        Concatenator concatenator("compil_data/" + std::to_string(i) + "_sausage.flv", {
            { "compil_data/timelapse20191203-16.flv", FROM_START, TML(7464) }
            , { "compil_data/event20191203-16-35-16-0.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(26694), TML(30362) }
            , { "compil_data/event20191203-16-35-39-1.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(61657), TML(68286) }
            , { "compil_data/event20191203-16-36-17-2.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(91502), TML(97964) }
            , { "compil_data/event20191203-16-36-47-3.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(104750), TML(115432) }
            , { "compil_data/event20191203-16-37-04-4.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(155211), TML(164851) }
            , { "compil_data/event20191203-16-37-54-5.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(170806), TML(185490) }
            , { "compil_data/event20191203-16-38-14-6.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(195335), TML(200835) }
            , { "compil_data/event20191203-16-38-30-7.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(208656), TML(213171) }
            , { "compil_data/event20191203-16-38-42-8.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(222016), 225574 }
            , { "compil_data/event20191203-16-38-54-9.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(231545), TML(243085) }
            , { "compil_data/event20191203-16-39-12-10.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(249837), TML(257425) }
            , { "compil_data/event20191203-16-39-26-11.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(297203), TML(313058) }
            , { "compil_data/event20191203-16-40-22-12.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(318974), TML(342773) }
            , { "compil_data/event20191203-16-40-52-13.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(370179), TML(373755) }
            , { "compil_data/event20191203-16-41-23-14.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(380534), TML(390187) }
            , { "compil_data/event20191203-16-41-39-15.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(396093), TML(400531) }
            , { "compil_data/event20191203-16-41-49-16.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(440347), TML(444174) }
            , { "compil_data/event20191203-16-42-33-17.flv", FROM_START, TO_END }
            , { "compil_data/timelapse20191203-16.flv", TML(450116), TML(460103) }
        });

        if (auto ret = concatenator.start(); ret != Code::OK) {
            static_log_error("main", "Concatenator start failed: " << ret << " - " << utils::code_to_string(ret));
        }
        concatenator.join();
    }

    return 0;

//    CustomPacketSource test("", {}, [](Packet&){return Code::OK;}, [](Packet&){return Code::OK;});
//    test.stream()


    try {
//        set_log_level(LogLevel::Debug);
//        set_log_level(LogLevel::Trace);
//        set_log_level(LogLevel::Quiet);

        set_ffmpeg_log_level(LogLevel::Quiet);
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

            if (auto ret = pipeline->start(); ret != Code::OK) {
                static_log_error("main", "Pipeline start failed: " << ret << " - " << utils::code_to_string(ret));
            }

//            auto sink_event = new MediaSink("group_video/eventIP.flv", IOType::Event);
//            pipeline->addElement(sink_event);

            auto sink_timelapse = new MediaSink("group_video/timelapseIP.flv", IOType::Timelapse);
            pipeline->addElement(sink_timelapse);

//            auto sink_timelapse_2 = new MediaSink("group_video/timelapseIP2.flv", IOType::Timelapse);
//            pipeline->addElement(sink_timelapse_2);

//            auto params = new VideoParameters;
//            params->setCodec("libx264", CodecType::Encoder);
//            params->setStreamIndex(0);
//            auto custom_sink = new CustomPacketSink {
//                        "test"
//                        , { new VideoStream(params) }
//                        , [](Packet& packet) {
//                            UNUSED(packet);
//                            return Code::OK;
//                        }
//                        , [](Packet& packet) {
//                            UNUSED(packet);
//                            return Code::OK;
//                        }
//            };
//            pipeline->addElement(custom_sink);

//            pipeline->simplifyRoutes();
            utils::sleep_for_min(1);
//            delete sink_event;
//            delete sink_timelapse;
        }

//        {
//            Pipeline* pipeline = new Pipeline;
//            auto source = new MediaSource("input.flv");
////            source->setCloseOnDisconnect(false);
//            pipeline->addElement(source);

//            auto sink_event = new MediaSink("output.flv", IOType::Event);
//            pipeline->addElement(sink_event);

//            if (auto ret = pipeline->start(); ret != Code::OK) {
//                static_log_error("main", "Pipeline start failed: " << ret << " - " << utils::code_to_string(ret));
//            }

//            pipeline->join();

////            utils::sleep_for_min(1);
//            utils::sleep_for_ms(10);
//            delete sink_event;
////            delete sink_timelapse;
//        }

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
