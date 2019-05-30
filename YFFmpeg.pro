TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17

SOURCES += \
    base\YAbstractFilter.cpp \
    YAudioFilter.cpp \
    YAudioParameters.cpp \
    YAudioResampler.cpp \
    base\YFrame.cpp \
    base\YPacket.cpp \
    base\YParameters.cpp \
    YVideoFilter.cpp \
    YVideoParameters.cpp \
    YVideoRescaler.cpp \
        main.cpp \
    YMediaSource.cpp \
    YMediaDestination.cpp \
    base\YAbstractMedia.cpp \
    YMediaChain.cpp \
    YMediaDecoder.cpp \
    YMediaDestination.cpp \
    YMediaEncoder.cpp \
    YMediaFilter.cpp \
    YMediaSource.cpp \
    base\YAbstractCoder.cpp

HEADERS += \
    base\YAbstractFilter.h \
    YAudioFilter.h \
    YAudioParameters.h \
    YAudioResampler.h \
    base\YFrame.h \
    YMediaSource.h \
    YMediaDestination.h \
    base\YAbstractMedia.h \
    YMediaChain.h \
    YMediaDecoder.h \
    YMediaDestination.h \
    YMediaEncoder.h \
    YMediaFilter.h \
    YMediaSource.h \
    base\YAbstractCoder.h \
    base\YPacket.h \
    base\YParameters.h \
    YVideoFilter.h \
    YVideoParameters.h \
    YVideoRescaler.h \
    base\ffmpeg.h

#-- Home --
INCLUDEPATH += G:\dev\FFmpeg\ffmpeg-4.1.1-win64-dev\include
LIBS += -LG:\dev\FFmpeg\ffmpeg-4.1.1-win64-dev\lib

#-- Office 1 --
INCLUDEPATH += D:\dev\00_ffmpeg\ffmpeg-4.1.1-win64-dev\include
LIBS += -LD:\dev\00_ffmpeg\ffmpeg-4.1.1-win64-dev\lib

#-- Office 2 --
INCLUDEPATH += D:\dev\00_ffmpeg\ffmpeg-4.1.3-win64-dev\include
LIBS += -LD:\dev\00_ffmpeg\ffmpeg-4.1.3-win64-dev\lib

LIBS += -lavcodec -lavformat -lavutil -lswscale -lavdevice -lavfilter -lpostproc -lswresample
