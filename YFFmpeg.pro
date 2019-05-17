TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17

SOURCES += \
        main.cpp \
    YMediaSource.cpp \
    YMediaDestination.cpp \
    YAbstractMedia.cpp \
    YMediaChain.cpp \
    YMediaDecoder.cpp \
    YMediaDestination.cpp \
    YMediaEncoder.cpp \
    YMediaFilter.cpp \
    YMediaSource.cpp \
    YAbstractCoder.cpp \
    avcpp/audioresampler.cpp \
    avcpp/averror.cpp \
    avcpp/avtime.cpp \
    avcpp/avutils.cpp \
    avcpp/codec.cpp \
    avcpp/codeccontext.cpp \
    avcpp/dictionary.cpp \
    avcpp/format.cpp \
    avcpp/formatcontext.cpp \
    avcpp/frame.cpp \
    avcpp/packet.cpp \
    avcpp/pixelformat.cpp \
    avcpp/rational.cpp \
    avcpp/rect.cpp \
    avcpp/sampleformat.cpp \
    avcpp/stream.cpp \
    avcpp/timestamp.cpp \
    avcpp/videorescaler.cpp \
    avcpp/filters/buffersink.cpp \
    avcpp/filters/buffersrc.cpp \
    avcpp/filters/filter.cpp \
    avcpp/filters/filtercontext.cpp \
    avcpp/filters/filtergraph.cpp \
    avcpp/filters/filterpad.cpp

HEADERS += \
    YMediaSource.h \
    YMediaDestination.h \
    YAbstractMedia.h \
    YMediaChain.h \
    YMediaDecoder.h \
    YMediaDestination.h \
    YMediaEncoder.h \
    YMediaFilter.h \
    YMediaSource.h \
    YAbstractCoder.h \
    ffmpeg.h \
    avcpp/audioresampler.h \
    avcpp/av.h \
    avcpp/averror.h \
    avcpp/avlog.h \
    avcpp/avtime.h \
    avcpp/avutils.h \
    avcpp/codec.h \
    avcpp/codeccontext.h \
    avcpp/dictionary.h \
    avcpp/ffmpeg.h \
    avcpp/format.h \
    avcpp/formatcontext.h \
    avcpp/frame.h \
    avcpp/linkedlistutils.h \
    avcpp/packet.h \
    avcpp/pixelformat.h \
    avcpp/rational.h \
    avcpp/rect.h \
    avcpp/sampleformat.h \
    avcpp/stream.h \
    avcpp/timestamp.h \
    avcpp/videorescaler.h \
    avcpp/filters/buffersink.h \
    avcpp/filters/buffersrc.h \
    avcpp/filters/filter.h \
    avcpp/filters/filtercontext.h \
    avcpp/filters/filtergraph.h \
    avcpp/filters/filterpad.h

#--Home--
INCLUDEPATH += G:\dev\FFmpeg\ffmpeg-4.1.1-win64-dev\include
LIBS += -LG:\dev\FFmpeg\ffmpeg-4.1.1-win64-dev\lib

#--Office--
INCLUDEPATH += D:\dev\00_ffmpeg\ffmpeg-4.1.1-win64-dev\include
LIBS += -LD:\dev\00_ffmpeg\ffmpeg-4.1.1-win64-dev\lib

LIBS += -lavcodec -lavformat -lavutil -lswscale -lavdevice -lavfilter -lpostproc -lswresample

DISTFILES += \
    avcpp/libavcpp.pc.in \
    avcpp/CMakeLists.txt
