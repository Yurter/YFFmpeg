TEMPLATE = app

#TEMPLATE = lib
#CONFIG += static

TARGET = fpp
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17

CONFIG(release, debug|release) {
    TARGET = fpp_r
}

CONFIG(debug, debug|release) {
    TARGET = fpp
}

QMAKE_CXXFLAGS += /std:c++latest #Домашний креатор не может использовать c++17 атрибут nodiscard без этого флага

SOURCES += \
    base/CodecContext.cpp \
    control/Route.cpp \
    codec/Decoder.cpp \
    codec/DecoderContext.cpp \
    codec/Encoder.cpp \
    codec/EncoderContext.cpp \
    control/Concatenator.cpp \
    custom/CustomFrameSink.cpp \
    custom/CustomFrameSource.cpp \
    custom/CustomPacketSink.cpp \
    custom/CustomPacketSource.cpp \
    format/InputFormatContext.cpp \
    format/OutputFormatContext.cpp \
    core/Exception.cpp \
    inout/FrameProducer.cpp \
    inout/FrameSink.cpp \
    inout/PacketProcessor.cpp \
    base/Filter.cpp \
    inout/FrameProcessor.cpp \
    base/FormatContext.cpp \
    core/Logger.cpp \
    core/Object.cpp \
    base/Processor.cpp \
    core/Thread.cpp \
    inout/FrameSource.cpp \
    inout/PacketProducer.cpp \
    media/MediaSink.cpp \
    media/MediaSource.cpp \
    inout/PacketSink.cpp \
    inout/PacketSource.cpp \
    opencv/OpenCVSink.cpp \
    stream/AudioParameters.cpp \
    control/Pipeline.cpp \
    stream/VideoParameters.cpp \
    inout/FrameSource.cpp \
    refi/AudioFilter.cpp \
    refi/Resampler.cpp \
    stream/Stream.cpp \
    core/utils.cpp \
    main.cpp \
    base/Frame.cpp \
    base/Packet.cpp \
    base/Parameters.cpp \
    refi/Rescaler.cpp \
    refi/VideoFilter.cpp

HEADERS += \
    base/CodecContext.hpp \
    control/Route.hpp \
    codec/Decoder.hpp \
    codec/DecoderContext.hpp \
    codec/Encoder.hpp \
    codec/EncoderContext.hpp \
    control/Concatenator.hpp \
    core/async/AsyncList.hpp \
    core/async/AsyncObject.hpp \
    core/async/AsyncVector.hpp \
    core/time/Chronometer.hpp \
    core/time/Timer.hpp \
    custom/CustomFrameSink.hpp \
    custom/CustomFrameSource.hpp \
    custom/CustomPacketSink.hpp \
    custom/CustomPacketSource.hpp \
    format/InputFormatContext.hpp \
    format/OutputFormatContext.hpp \
    core/async/AsyncQueue.hpp \
    base/Data.hpp \
    base/FormatContext.hpp \
    base/TemplateProcessor.hpp \
    base/TemplateProcessor.hpp \
    core/Exception.hpp \
    inout/FrameProducer.hpp \
    inout/PacketProcessor.hpp \
    base/Filter.hpp \
    inout/FrameProcessor.hpp \
    core/Logger.hpp \
    base/MediaData.hpp \
    core/Object.hpp \
    base/Processor.hpp \
    stream/Stream.hpp \
    core/Thread.hpp \
    core/utils.hpp \
    inout/FrameSource.hpp \
    inout/PacketProducer.hpp \
    media/MediaSink.hpp \
    media/MediaSource.hpp \
    inout/PacketSink.hpp \
    inout/PacketSource.hpp \
    opencv/OpenCVSink.hpp \
    stream/AudioParameters.hpp \
    control/Pipeline.hpp \
    stream/VideoParameters.hpp \
    fpp.hpp \
    inout/FrameSink.hpp \
    inout/FrameSource.hpp \
    refi/AudioFilter.hpp \
    refi/Resampler.hpp \
    core/ffmpeg.hpp \
    base/Frame.hpp \
    base/Packet.hpp \
    base/Parameters.hpp \
    refi/Rescaler.hpp \
    refi/VideoFilter.hpp

#FFmpeg
INCLUDEPATH += D:\dev\00_ffmpeg\ffmpeg-4.1.3-win64-dev\include
LIBS += -LD:\dev\00_ffmpeg\ffmpeg-4.1.3-win64-dev\lib
LIBS += -lavcodec -lavformat -lavutil -lswscale -lavdevice
LIBS += -lavfilter -lpostproc -lswresample

#OpenCV
INCLUDEPATH += D:\dev\00_opencv\410\build\include
LIBS += -LD:\dev\00_opencv\410\build\x64\vc15\lib
LIBS += -LD:\dev\00_opencv\410\build\x64\vc15\bin
LIBS += -lopencv_world410 -lopencv_world410d
