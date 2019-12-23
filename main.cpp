#include "fpp.hpp"

using namespace std;
using namespace fpp;

void progress_bar(int sec) {
    int msec_interval = sec * 10;
    static_log_info("main", "Progress:");
    for (int i = 0; i < 100; ++i) {
        std::cout << "\r" << "[" << std::string(size_t(i), '>') << std::string(size_t(100 - i), ' ') << "]";
        utils::sleep_for_ms(msec_interval);
    }
    std::cout << std::endl;
}

void concatenator_debug_simple() {
    Concatenator concatenator("debug_files/simple_sausage.flv", {
        { "5sec.flv", FROM_START, TO_END }
        , { "5sec.flv", FROM_START, TO_END }
        , { "5sec.flv", FROM_START, TO_END }
        , { "5sec.flv", FROM_START, TO_END }
        , { "5sec.flv", FROM_START, TO_END }
    });

    if (auto ret = concatenator.start(); ret != Code::OK) {
        static_log_error("main", "Concatenator start failed: " << ret << " - " << utils::code_to_string(ret));
    }
    concatenator.join();
}

void concatenator_debug_timlapse_event() {
    #define TML(x) (x) / 60
    #define MINUTE * 60'000

    // 58'021 ms длительность таймлапса
//    Concatenator concatenator("debug_files/concatenator_debug_timlapse_event.flv", {
//        { "event.flv", (10 * MINUTE), (11 * MINUTE) }
//    });
//    Concatenator concatenator("debug_files/concatenator_debug_timlapse_event.flv", {
//        { "event.flv", FROM_START, TO_END }
//    });
//    Concatenator concatenator("compil_data/concatenator_debug_timlapse_event.flv", {
//        { "a_event.flv", (0 * MINUTE), (11 * MINUTE) }
//    });
    Concatenator concatenator("debug_files/concatenator_debug_timlapse_event.flv", {
        { "timelapse.flv", FROM_START, TML(10 MINUTE) }         /* 10 сек */
        , { "event.flv", (10 MINUTE), (11 MINUTE) }             /*  1 мин */
        , { "timelapse.flv", TML(11 MINUTE), TML(15 MINUTE) }   /*  4 сек */
        , { "event.flv", (15 MINUTE), (16 MINUTE) }             /*  1 мин */
        , { "timelapse.flv", TML(16 MINUTE), TML(23 MINUTE) }   /*  7 сек */
        , { "event.flv", (23 MINUTE), (24 MINUTE) }             /*  1 мин */
        , { "timelapse.flv", TML(24 MINUTE), TO_END }           /*  ? сек */
                                                        /* Итого: 3 мин 21 сек + */
    });

    if (auto ret = concatenator.start(); ret != Code::OK) {
        static_log_error("main", "Concatenator start failed: " << ret << " - " << utils::code_to_string(ret));
    }
    concatenator.join();
}

void copy_file_debug() {
    PipelinePointer pipeline = std::make_shared<Pipeline>();

    ProcessorPointer source = std::make_shared<MediaSource>("out.flv");
    if (auto ret = pipeline->addElement(source); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline add source failed: " << ret << " - " << utils::code_to_string(ret));
    }

    ProcessorPointer sink_copy = std::make_shared<MediaSink>("debug_files/a_event_copy.flv", IOType::Event);
    if (auto ret = pipeline->addElement(sink_copy); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline add sink_copy failed: " << ret << " - " << utils::code_to_string(ret));
    }

    if (auto ret = pipeline->start(); ret != Code::OK) {
        static_log_error("main", "Concatenator start failed: " << ret << " - " << utils::code_to_string(ret));
    }

    pipeline->join();
}

void seek_debug() {

}

void concatenator_debug() {
    #define TML(x) (x) / 60

    int iter_amount = 1;
    for (int i = 0; i < iter_amount; ++i) {
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
}

void event_debug() {
    PipelinePointer pipeline = std::make_shared<Pipeline>();

    ProcessorPointer source = std::make_shared<MediaSource>("rtsp://admin:Admin2019@192.168.10.12:554");
    if (auto ret = pipeline->addElement(source); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline add source failed: " << ret << " - " << utils::code_to_string(ret));
    }

    ProcessorPointer sink_event = std::make_shared<MediaSink>("group_video/event.flv", IOType::Event);
    if (auto ret = pipeline->addElement(sink_event); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline add sink_event failed: " << ret << " - " << utils::code_to_string(ret));
    }

    utils::sleep_for_sec(30);
}

void timelapse_debug() {
    PipelinePointer pipeline = std::make_shared<Pipeline>();

    ProcessorPointer source = std::make_shared<MediaSource>("a_event.flv");
    if (auto ret = pipeline->addElement(source); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline add source failed: " << ret << " - " << utils::code_to_string(ret));
    }

    ProcessorPointer sink_timelapse = std::make_shared<MediaSink>("group_video/timelapse.flv", IOType::Timelapse);
    if (auto ret = pipeline->addElement(sink_timelapse); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline add sink_timelapse failed: " << ret << " - " << utils::code_to_string(ret));
    }

    utils::sleep_for_sec(30);
}

void route_simplify_debug() {
    PipelinePointer pipeline = std::make_shared<Pipeline>();

    ProcessorPointer source = std::make_shared<MediaSource>("rtsp://admin:admin@192.168.10.3:554");
    source->setCloseOnDisconnect(false);
    if (auto ret = pipeline->addElement(source); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline add source failed: " << ret << " - " << utils::code_to_string(ret));
    }

    for (int i = 0; i < 5; ++i) {
        ProcessorPointer sink_event = std::make_shared<MediaSink>("debug_files/" + std::to_string(i) + "_event.flv", IOType::Event);
        if (auto ret = pipeline->addElement(sink_event); ret != fpp::Code::OK) {
            static_log_error("main", "Pipeline add sink_event failed: " << ret << " - " << utils::code_to_string(ret));
        }
    }

    if (auto ret = pipeline->simplifyRoutes(); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline simplify routes failed: " << ret << " - " << utils::code_to_string(ret));
    }

    progress_bar(60);
}

void memory_leak_test() {
    PipelinePointer pipeline = std::make_shared<Pipeline>();

    ProcessorPointer source = std::make_shared<MediaSource>("video=HP Wide Vision FHD Camera");
    source->setCloseOnDisconnect(false);

    if (auto ret = pipeline->addElement(source); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline add source failed: " << ret << " - " << utils::code_to_string(ret));
    }

//    ProcessorPointer sink_timelapse = std::make_shared<MediaSink>("group_video/timelapse.flv", IOType::Timelapse);
//    if (auto ret = pipeline->addElement(sink_timelapse); ret != fpp::Code::OK) {
//        static_log_error("main", "Pipeline add sink_timelapse failed: " << ret << " - " << utils::code_to_string(ret));
//    }

    int counter = 0;
    while (true) {
        ProcessorPointer sink_event = std::make_shared<MediaSink>("group_video/" + std::to_string(counter++) + "_event.flv", IOType::Event);
        if (auto ret = pipeline->addElement(sink_event); ret != fpp::Code::OK) {
            static_log_error("main", "Pipeline add sink_event failed: " << ret << " - " << utils::code_to_string(ret));
        }
//        sink_event->disconnectFrom(sink_event);
        utils::sleep_for_sec(10);
        pipeline->remElement(sink_event->uid());
    }
}

void write_video_from_camera() {
    PipelinePointer pipeline = std::make_shared<Pipeline>();

    ProcessorPointer source = std::make_shared<MediaSource>("rtsp://admin:admin@192.168.10.3:554");

    if (auto ret = pipeline->addElement(source); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline add source failed: " << ret << " - " << utils::code_to_string(ret));
    }

    ProcessorPointer sink_event = std::make_shared<MediaSink>("debug_files/event.flv", IOType::Event);
    if (auto ret = pipeline->addElement(sink_event); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline add sink_event failed: " << ret << " - " << utils::code_to_string(ret));
    }

    ProcessorPointer sink_timelapse = std::make_shared<MediaSink>("debug_files/timelapse.flv", IOType::Timelapse);
    if (auto ret = pipeline->addElement(sink_timelapse); ret != fpp::Code::OK) {
        static_log_error("main", "Pipeline add sink_timelapse failed: " << ret << " - " << utils::code_to_string(ret));
    }

//    utils::sleep_for_min(60);
    progress_bar(30 * 60);
}

int main() {

    // video=HP Wide Vision FHD Camera
    // video=Webcam C170
    // video=USB2.0 PC CAMERA
    // rtsp://admin:admin@192.168.10.189:554/ch01.264
    // rtsp://admin:Admin2019@192.168.10.12:554
    // rtsp://admin:admin@192.168.10.3:554

    static_log_info("main", "Program started...");
    avdevice_register_all(); //TODO вызов необходим для работы USB-камер, перенести

//    {
//        set_log_level(LogLevel::Debug);
//        event_debug();
//        return 0;
//    }

//    {
//        set_log_level(LogLevel::Debug);
//        timelapse_debug();
//        return 0;
//    }

//    {
//        set_log_level(LogLevel::Debug);
//        concatenator_debug_simple();
//        return 0;
//    }

    {
        set_log_level(LogLevel::Debug);
        concatenator_debug_timlapse_event();
        return 0;
    }

//    {
//        set_log_level(LogLevel::Debug);
//        set_ffmpeg_log_level(LogLevel::Quiet);
//        write_video_from_camera();
//        return 0;
//    }

//    {
//        set_log_level(LogLevel::Debug);
//        copy_file_debug();
//        return 0;
//    }

//    {
//        set_log_level(LogLevel::Debug);
//        route_simplify_debug();
//        return 0;
//    }

//    {
//        set_log_level(LogLevel::Debug);
//        memory_leak_test();
//        return 0;
//    }

    static_log_info("main", "Program finished");

    return 0;

}
