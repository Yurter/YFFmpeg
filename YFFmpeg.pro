TEMPLATE = app
#TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17

SOURCES += \
    base/Context.cpp \
    base/Exception.cpp \
    base/Filter.cpp \
    base/FrameProcessor.cpp \
    base/Logger.cpp \
    base/MediaData.cpp \
    base/Object.cpp \
    base/Refi.cpp \
    base/Thread.cpp \
    codec/Decoder.cpp \
    codec/Encoder.cpp \
    context/RawSink.cpp \
    context/RawSource.cpp \
    context/Sink.cpp \
    context/Source.cpp \
    control/AudioParameters.cpp \
    control/AudioStream.cpp \
    control/Pipeline.cpp \
    control/VideoParameters.cpp \
    control/VideoStream.cpp \
    control/YMap.cpp \
    refi/AudioFilter.cpp \
    refi/MediaFilter.cpp \
    refi/Resampler.cpp \
    base/Stream.cpp \
    base/utils.cpp \
    main.cpp \
    base/Context.cpp \
    base/Filter.cpp \
    base/Frame.cpp \
    base/Packet.cpp \
    base/Parameters.cpp \
    refi/Rescaler.cpp \
    refi/VideoFilter.cpp

HEADERS += \
    base/AsyncQueue.hpp \
    base/Codec.hpp \
    base/Context.hpp \
    base/Data.hpp \
    base/DataProcessor.hpp \
    base/Exception.hpp \
    base/Filter.hpp \
    base/FrameProcessor.hpp \
    base/Logger.hpp \
    base/MediaData.hpp \
    base/Object.hpp \
    base/Refi.hpp \
    base/Stream.hpp \
    base/Thread.hpp \
    base/utils.hpp \
    codec/Decoder.hpp \
    codec/Encoder.hpp \
    context/RawSink.hpp \
    context/RawSource.hpp \
    context/Sink.hpp \
    context/Source.hpp \
    control/AudioParameters.hpp \
    control/AudioStream.hpp \
    control/Pipeline.hpp \
    control/VideoParameters.hpp \
    control/VideoStream.hpp \
    control/YMap.hpp \
    fpp.hpp \
    refi/AudioFilter.hpp \
    refi/MediaFilter.hpp \
    refi/Resampler.hpp \
    base/ffmpeg.hpp \
    base/Codec.hpp \
    base/Context.hpp \
    base/YFilter.hpp \
    base/Frame.hpp \
    base/Packet.hpp \
    base/Parameters.hpp \
    refi/Rescaler.hpp \
    refi/VideoFilter.hpp

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

#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg
