#include <iostream>
#include "YMediaChain.h"

using namespace std;

int main()
{
//    av_log_set_level(AV_LOG_VERBOSE);
//    av_log_set_level(AV_LOG_DEBUG);
    cout << "Program started..." << endl;


    /* Запись rtsp с камеры в flv/YouTube */
//    std::string mrl_src = "rtsp://admin:Admin2019@192.168.10.12";
    std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
//    std::string mrl_src = "rtsp://192.168.0.14:8080/h264";

//    std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/ryyg-4xst-30h5-5g7z";
    std::string mrl_dst = "remuxed.flv";

    YSource source(mrl_src);
//    source.audio_parameters.setIgnore(true);
    YDestination destination(mrl_dst, YMediaPreset::YouTube);

    YMediaChain chain(&source, &destination, COPY_VIDEO);


//    source.open();
//    source.start();
//    while (true) {
//        //
//    }



    /* Запись белого шума из lavfi в aac */
//    std::string mrl_src = "aevalsrc=-2+random(0)";
//    std::string mrl_dst = "remuxed.aac";

//    YSource source(mrl_src, YMediaPreset::Virtual);
//    YDestination destination(mrl_dst);

//    destination.audio_parameters.setSampleRate(44'100);
//    destination.audio_parameters.setSampleFormat(AV_SAMPLE_FMT_FLTP);
//    destination.audio_parameters.setBitrate(128'000);
//    destination.audio_parameters.setChanelsLayout(AV_CH_LAYOUT_STEREO);
//    destination.audio_parameters.setChanels(2);
//    destination.audio_parameters.setCodec("aac");
//    destination.audio_parameters.setAvailable(true);

//    YMediaChain chain(&source, &destination);




    /* Запись звука с камеры в aac */
//    std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
//    std::string mrl_dst = "remuxed.aac";

//    YSource source(mrl_src);
//    YDestination destination(mrl_dst);

//    source.video_parameters.setIgnore(true);
//    destination.audio_parameters.setSampleRate(44'100);
//    destination.audio_parameters.setSampleFormat(AV_SAMPLE_FMT_FLTP);
//    destination.audio_parameters.setBitrate(128'000);
//    destination.audio_parameters.setChanelsLayout(AV_CH_LAYOUT_STEREO);
//    destination.audio_parameters.setChanels(2);
//    destination.audio_parameters.setCodec("aac");
//    destination.audio_parameters.setAvailable(true);

//    YMediaChain chain(&source, &destination);



//    chain.init();
    chain.start();
    while (chain.active()) {
        utils::sleep_for(LONG_DELAY_MS);
    }

    cout << "Program finished." << endl;
    return 0;

}
