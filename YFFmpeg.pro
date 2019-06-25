TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17

SOURCES += \
    base/YAbstractRefi.cpp \
    codec/YDecoder.cpp \
    codec/YEncoder.cpp \
    control/YAudioStream.cpp \
    context/YDestination.cpp \
    refi/YResampler.cpp \
    refi/YRescaler.cpp \
    context/YSource.cpp \
    control/YStreamMap.cpp \
    control/YVideoStream.cpp \
    base/YObject.cpp \
    base/YStream.cpp \
    base/YThread.cpp \
    base/logger.cpp \
    base/utils.cpp \
    main.cpp \
    base\YAbstractCodec.cpp \
    base\YAbstractMedia.cpp \
    base\YAbstractFilter.cpp \
    base\YFrame.cpp \
    base\YPacket.cpp \
    base\YParameters.cpp \
    control/YVideoParameters.cpp \
    control/YAudioParameters.cpp \
    control/YFFmpeg.cpp \
    refi/YVideoFilter.cpp \
    refi/YAudioFilter.cpp \
    refi/YMediaFilter.cpp \

HEADERS += \
    base/Logger.h \
    base/YAbstractRefi.h \
    codec/YDecoder.h \
    codec/YEncoder.h \
    control/YAudioStream.h \
    context/YDestination.h \
    control/YFFmpeg.h \
    refi/YResampler.h \
    refi/YRescaler.h \
    context/YSource.h \
    control/YStreamMap.h \
    control/YVideoStream.h \
    base/YAsyncQueue.h \
    base/YData.h \
    base/YDataProcessor.h \
    base/YObject.h \
    base/YStream.h \
    base/YThread.h \
    base/utils.h \
    base\ffmpeg.h \
    base\YAbstractCodec.h \
    base\YAbstractMedia.h \
    base\YAbstractFilter.h \
    base\YFrame.h \
    base\YPacket.h \
    base\YParameters.h \
    control/YVideoParameters.h \
    control/YAudioParameters.h \
    refi/YMediaFilter.h \
    refi/YAudioFilter.h \
    refi/YVideoFilter.h \

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
