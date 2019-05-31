TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17

SOURCES += \
    base/utils.cpp \
    main.cpp \
    base\YAbstractCoder.cpp \
    base\YAbstractMedia.cpp \
    base\YAbstractFilter.cpp \
    base\YFrame.cpp \
    base\YPacket.cpp \
    base\YParameters.cpp \
    YVideoParameters.cpp \
    YAudioParameters.cpp \
    YVideoRescaler.cpp \
    YAudioResampler.cpp \
    YMediaSource.cpp \
    YMediaDestination.cpp \
    YMediaChain.cpp \
    YMediaDecoder.cpp \
    YMediaDestination.cpp \
    YMediaEncoder.cpp \
    YVideoFilter.cpp \
    YAudioFilter.cpp \
    YMediaFilter.cpp \

HEADERS += \
    base/utils.h \
    base\ffmpeg.h \
    base\YAbstractCoder.h \
    base\YAbstractMedia.h \
    base\YAbstractFilter.h \
    base\YFrame.h \
    base\YPacket.h \
    base\YParameters.h \
    YVideoParameters.h \
    YAudioParameters.h \
    YVideoRescaler.h \
    YAudioResampler.h \
    YMediaSource.h \
    YMediaDestination.h \
    YMediaChain.h \
    YMediaDecoder.h \
    YMediaDestination.h \
    YMediaEncoder.h \
    YMediaFilter.h \
    YAudioFilter.h \
    YVideoFilter.h \

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
