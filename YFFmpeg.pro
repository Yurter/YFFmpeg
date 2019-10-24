TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17

SOURCES += \
    base/Codec.cpp \
    base/InputFormatContext.cpp \
    base/OutputFormatContext.cpp \
    core/Exception.cpp \
    inout/PacketProcessor.cpp \
    refi/Filter.cpp \
    inout/FrameProcessor.cpp \
    base/FormatContext.cpp \
    core/Logger.cpp \
    base/MediaData.cpp \
    core/Object.cpp \
    base/Processor.cpp \
    core/Thread.cpp \
    codec/Decoder.cpp \
    codec/Encoder.cpp \
    inout/FrameSource.cpp \
    media/MediaSink.cpp \
    media/MediaSource.cpp \
    inout/PacketSink.cpp \
    inout/PacketSource.cpp \
    control/AudioParameters.cpp \
    control/AudioStream.cpp \
    control/Pipeline.cpp \
    control/VideoParameters.cpp \
    control/VideoStream.cpp \
    control/YMap.cpp \
    inout/FrameSource.cpp \
    refi/AudioFilter.cpp \
    refi/Resampler.cpp \
    base/Stream.cpp \
    core/utils.cpp \
    main.cpp \
    base/Frame.cpp \
    base/Packet.cpp \
    base/Parameters.cpp \
    refi/Rescaler.cpp \
    refi/VideoFilter.cpp

HEADERS += \
    base/InputFormatContext.hpp \
    base/OutputFormatContext.hpp \
    core/AsyncQueue.hpp \
    base/Codec.hpp \
    base/Data.hpp \
    base/FormatContext.hpp \
    base/TemplateProcessor.hpp \
    base/TemplateProcessor.hpp \
    core/Exception.hpp \
    inout/PacketProcessor.hpp \
    refi/Filter.hpp \
    inout/FrameProcessor.hpp \
    core/Logger.hpp \
    base/MediaData.hpp \
    core/Object.hpp \
    base/Processor.hpp \
    base/Stream.hpp \
    core/Thread.hpp \
    core/utils.hpp \
    codec/Decoder.hpp \
    codec/Encoder.hpp \
    inout/FrameSource.hpp \
    media/MediaSink.hpp \
    media/MediaSource.hpp \
    inout/PacketSink.hpp \
    inout/PacketSource.hpp \
    control/AudioParameters.hpp \
    control/AudioStream.hpp \
    control/Pipeline.hpp \
    control/VideoParameters.hpp \
    control/VideoStream.hpp \
    control/YMap.hpp \
    fpp.hpp \
    inout/FrameSink.hpp \
    inout/FrameSource.hpp \
    refi/AudioFilter.hpp \
    refi/Resampler.hpp \
    core/ffmpeg.hpp \
    base/Codec.hpp \
    base/Frame.hpp \
    base/Packet.hpp \
    base/Parameters.hpp \
    refi/Rescaler.hpp \
    refi/VideoFilter.hpp

#FFmpeg
#-- Home --
INCLUDEPATH += G:\dev\FFmpeg\ffmpeg-4.1.1-win64-dev\include
LIBS += -LG:\dev\FFmpeg\ffmpeg-4.1.1-win64-dev\lib

#-- Office --
INCLUDEPATH += D:\dev\00_ffmpeg\ffmpeg-4.1.3-win64-dev\include
LIBS += -LD:\dev\00_ffmpeg\ffmpeg-4.1.3-win64-dev\lib

LIBS += -lavcodec -lavformat -lavutil -lswscale -lavdevice -lavfilter -lpostproc -lswresample

#OpenCV
INCLUDEPATH += D:\dev\00_opencv\410\build\include
LIBS += -LD:\dev\00_opencv\410\build\x64\vc15\lib
LIBS += -LD:\dev\00_opencv\410\build\x64\vc15\bin
LIBS += -lopencv_world410

