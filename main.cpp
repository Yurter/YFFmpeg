#include <iostream>
#include "YMediaChain.h"

using namespace std;

int main()
{

    av_log_set_level(AV_LOG_VERBOSE);
//    av_log_set_level(AV_LOG_DEBUG);
    cout << "Program started..." << endl;


    /* Запись rtsp с камеры в flv/YouTube */
//    std::string mrl_src = "rtsp://admin:Admin2019@192.168.10.12";
    std::string mrl_src = "rtsp://admin:admin@192.168.10.3";
//    std::string mrl_src = "rtsp://192.168.0.14:8080/h264";

//    std::string mrl_dst = "rtmp://a.rtmp.youtube.com/live2/ryyg-4xst-30h5-5g7z";
    std::string mrl_dst = "remuxed.flv";

    YMediaSource source(mrl_src);
//    source.audio_parameters.setIgnore(true);
    YMediaDestination destination(mrl_dst, YMediaPreset::YouTube);

    YMediaChain chain(&source, &destination, COPY_VIDEO);





    /* Запись белого шума из lavfi в aac */
//    std::string mrl_src = "aevalsrc=-2+random(0)";
//    std::string mrl_dst = "remuxed.aac";

//    YMediaSource source(mrl_src, YMediaPreset::Virtual);
//    YMediaDestination destination(mrl_dst);

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

//    YMediaSource source(mrl_src);
//    YMediaDestination destination(mrl_dst);

//    source.video_parameters.setIgnore(true);
//    destination.audio_parameters.setSampleRate(44'100);
//    destination.audio_parameters.setSampleFormat(AV_SAMPLE_FMT_FLTP);
//    destination.audio_parameters.setBitrate(128'000);
//    destination.audio_parameters.setChanelsLayout(AV_CH_LAYOUT_STEREO);
//    destination.audio_parameters.setChanels(2);
//    destination.audio_parameters.setCodec("aac");
//    destination.audio_parameters.setAvailable(true);

//    YMediaChain chain(&source, &destination);




    chain.start();
    while (chain.active()) {
        utils::sleep_for(LONG_DELAY_MS);
    }

    cout << "Program finished." << endl;
    return 0;

}

/* - Консольный FFmpeg - vcopy + resample = 1-4%, 4000-6000K
Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/tvtx-ap3v-s388-ddza':
  Metadata:
    title           : Media Server
    encoder         : Lavf58.20.100
    Stream #0:0: Video: flv1 (flv) ([2][0][0][0] / 0x0002), yuv420p, 1920x1080 [SAR 1:1 DAR 16:9], q=2-31, 200 kb/s, 25 fps, 1k tbn, 25 tbc
    Metadata:
      encoder         : Lavc58.35.100 flv
    Side data:
      cpb: bitrate max/min/avg: 0/0/200000 buffer size: 0 vbv_delay: -1
    Stream #0:1: Audio: mp3 (libmp3lame) ([2][0][0][0] / 0x0002), 44100 Hz, mono, s16p
    Metadata:
      encoder         : Lavc58.35.100 libmp3lame
*/
/*
Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/tvtx-ap3v-s388-ddza':
  Metadata:
    title           : Media Server
    encoder         : Lavf58.20.100
    Stream #0:0: Video: h264 (Main) ([7][0][0][0] / 0x0007), yuvj420p(pc, bt709, progressive), 1920x1080 [SAR 1:1 DAR 16:9], q=2-31, 23 fps, 25 tbr, 1k tbn, 90k tbc
    Stream #0:1: Audio: aac (LC) ([10][0][0][0] / 0x000A), 44100 Hz, mono, fltp, 69 kb/s
    Metadata:
      encoder         : Lavc58.35.100 aac
*/

/* - YFFmpeg - vcopy + resample = 25-27%, 25000-40000K
Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/tvtx-ap3v-s388-ddza':
  Metadata:
    encoder         : Lavf58.23.102
    Stream #0:0: Video: h264 (libx264) ([7][0][0][0] / 0x0007), yuv420p, 1920x1080 [SAR 1:1 DAR 16:9], q=-1--1, 400 kb/s, 23 fps, 23 tbr, 1k tbn
    Stream #0:1: Audio: aac (LC) ([10][0][0][0] / 0x000A), 44100 Hz, stereo, fltp, 128 kb/s
 */
