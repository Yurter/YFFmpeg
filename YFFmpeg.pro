TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17

SOURCES += \
    base/Codec.cpp \
    base/Exception.cpp \
    base/Filter.cpp \
    base/FrameProcessor.cpp \
    base/FrameProducer.cpp \
    base/IOContext.cpp \
    base/Logger.cpp \
    base/MediaData.cpp \
    base/Object.cpp \
    base/PacketProcessror.cpp \
    base/PacketProducer.cpp \
    base/Processor.cpp \
    base/Refi.cpp \
    base/Thread.cpp \
    codec/Decoder.cpp \
    codec/Encoder.cpp \
    context/FrameSource.cpp \
    context/MediaSink.cpp \
    context/MediaSource.cpp \
    context/PacketSink.cpp \
    context/PacketSource.cpp \
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
    base/Filter.cpp \
    base/Frame.cpp \
    base/Packet.cpp \
    base/Parameters.cpp \
    refi/Rescaler.cpp \
    refi/VideoFilter.cpp

HEADERS += \
    base/AsyncQueue.hpp \
    base/Codec.hpp \
    base/Data.hpp \
    base/FrameProducer.hpp \
    base/IOContext.hpp \
    base/PacketProcessor.hpp \
    base/PacketProducer.hpp \
    base/TemplateProcessor.hpp \
    base/TemplateProcessor.hpp \
    base/Exception.hpp \
    base/Filter.hpp \
    base/FrameProcessor.hpp \
    base/Logger.hpp \
    base/MediaData.hpp \
    base/Object.hpp \
    base/Processor.hpp \
    base/Refi.hpp \
    base/Stream.hpp \
    base/Thread.hpp \
    base/utils.hpp \
    codec/Decoder.hpp \
    codec/Encoder.hpp \
    context/FrameSource.hpp \
    context/MediaSink.hpp \
    context/MediaSource.hpp \
    context/PacketSink.hpp \
    context/PacketSource.hpp \
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
    base/YFilter.hpp \
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

