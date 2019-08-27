TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17

SOURCES += \
    base/YContext.cpp \
    base/YException.cpp \
    base/YFilter.cpp \
    base/YLogger.cpp \
    base/YMediaData.cpp \
    base/YRefi.cpp \
    codec/YDecoder.cpp \
    codec/YEncoder.cpp \
    control/YAudioStream.cpp \
    context/YDestination.cpp \
    control/YMap.cpp \
    refi/YResampler.cpp \
    refi/YRescaler.cpp \
    context/YSource.cpp \
    control/YVideoStream.cpp \
    base/YObject.cpp \
    base/YStream.cpp \
    base/YThread.cpp \
    base/utils.cpp \
    main.cpp \
    base\YContext.cpp \
    base\YFilter.cpp \
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
    base/YCodec.hpp \
    base/YContext.hpp \
    base/YException.hpp \
    base/YFilter.hpp \
    base/YLogger.hpp \
    base/YMediaData.hpp \
    base/YRefi.hpp \
    base/utils.hpp \
    codec/YDecoder.hpp \
    codec/YEncoder.hpp \
    control/YAudioStream.hpp \
    context/YDestination.hpp \
    control/YFFmpeg.hpp \
    control/YMap.hpp \
    refi/YResampler.hpp \
    refi/YRescaler.hpp \
    context/YSource.hpp \
    control/YVideoStream.hpp \
    base/YAsyncQueue.hpp \
    base/YData.hpp \
    base/YDataProcessor.hpp \
    base/YObject.hpp \
    base/YStream.hpp \
    base/YThread.hpp \
    base\ffmpeg.hpp \
    base\YCodec.hpp \
    base\YContext.hpp \
    base\YFilter.hpp \
    base\YFrame.hpp \
    base\YPacket.hpp \
    base\YParameters.hpp \
    control/YVideoParameters.hpp \
    control/YAudioParameters.hpp \
    refi/YMediaFilter.hpp \
    refi/YAudioFilter.hpp \
    refi/YVideoFilter.hpp \

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
