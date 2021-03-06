#include <iostream>
#include "YMediaChain.h"

using namespace std;

int main()
{
    av_log_set_level(AV_LOG_VERBOSE);
    cout << "Program started..." << endl;

//    YMediaSource source("rtsp://192.168.0.13:8080/video/h264");
//    YMediaSource source("G:\\dev\\video\\The Simpsons Movie - 1080p Trailer.mp4");
//    YMediaSource source("rtsp://admin:Admin2019@192.168.10.12");
//    YMediaSource source("rtsp://admin:admin@192.168.10.3");
//    YMediaDestination destination("rtmp://a.rtmp.youtube.com/live2/t7fv-q960-za3b-d1dr", YMediaDestination::YMediaPreset::YouTube);
    YMediaSource source("rtsp://admin:admin@192.168.10.3");
    YMediaFilter filter("scale=140:-1");
    YMediaDestination destination("findme.flv", YMediaDestination::YMediaPreset::YouTube);

    YMediaChain chain(&source, &filter, &destination);
    chain.start();
    while (chain.active()) {}

    cout << "Program finished." << endl;
    return 0;
}


/* Reference values:
Input #0, rtsp, from 'rtsp://192.168.0.13:8080/video/h264':
  Duration: N/A, start: -0.432396, bitrate: N/A
    Stream #0:0: Video: h264 (Baseline), yuv420p(tv, unknown/bt470bg/unknown, progressive), 640x480, 29.92 tbr, 90k tbn, 180k tbc
    Stream #0:1: Audio: aac (LC), 48000 Hz, mono, fltp
Overriding aspect ratio with stream copy may produce invalid files
Stream mapping:
  Stream #0:0 -> #0:0 (copy)
  Stream #0:1 -> #0:1 (aac (native) -> aac (native))
Press [q] to stop, [?] for help
Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/jky1-6yv3-t2h3-dyx2':
  Metadata:
    encoder         : Lavf58.20.100
    Stream #0:0: Video: h264 (Baseline) ([7][0][0][0] / 0x0007), yuv420p(tv, unknown/bt470bg/unknown, progressive), 640x480 [SAR 4:3 DAR 16:9], q=2-31, 29.92 tbr, 1k tbn, 90k tbc
    Stream #0:1: Audio: aac (LC) ([10][0][0][0] / 0x000A), 48000 Hz, mono, fltp, 69 kb/s
    Metadata:
      encoder         : Lavc58.35.100 aac
*/

/*
Output #0, flv, to 'rtmp://a.rtmp.youtube.com/live2/5frf-zuec-3ewr-d4fm':
  Metadata:
    encoder         : Lavf58.23.102
    Stream #0:0: Video: h264, 1 reference frame ([7][0][0][0] / 0x0007), yuv420p, 1920x1080 (0x0) [SAR 1:1 DAR 16:9], q=2-31, 200 kb/s, 1k tbn
    Stream #0:1: Audio: aac (LC) ([10][0][0][0] / 0x000A), 48000 Hz, mono, fltp, 70 kb/s
*/
