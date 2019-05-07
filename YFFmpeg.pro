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
    YAbstractCoder.cpp

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
    YAbstractCoder.h

#--Home--
#INCLUDEPATH += G:\dev\FFmpeg\ffmpeg-4.1.1-win64-dev\include
#LIBS += -LG:\dev\FFmpeg\ffmpeg-4.1.1-win64-dev\lib

#--Office--
INCLUDEPATH += D:\dev\00_ffmpeg\ffmpeg-4.1.1-win64-dev\include
LIBS += -LD:\dev\00_ffmpeg\ffmpeg-4.1.1-win64-dev\lib

LIBS += -lavcodec -lavformat -lavutil -lswscale -lavdevice -lavfilter -lpostproc -lswresample
