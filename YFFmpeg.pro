TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17

SOURCES += \
    YAudioParameters.cpp \
    YAudioResampler.cpp \
    YFrame.cpp \
    YPacket.cpp \
    YParameters.cpp \
    YVideoParameters.cpp \
    YVideoRescaler.cpp \
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
    YAbstractCoder.cpp

HEADERS += \
    YAudioParameters.h \
    YAudioResampler.h \
    YFrame.h \
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
    YPacket.h \
    YParameters.h \
    YVideoParameters.h \
    YVideoRescaler.h \
    ffmpeg.h

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
