TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17

QMAKE_CXXFLAGS += /std:c++latest #Домашний креатор не может использовать c++17 атрибут nodiscard без этого флага

SOURCES += \
    base/CodecContext.cpp \
    codec/Decoder.cpp \
    codec/DecoderContext.cpp \
    codec/Encoder.cpp \
    codec/EncoderContext.cpp \
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
    base/MediaData.cpp \
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
    stream/AudioStream.cpp \
    control/Pipeline.cpp \
    stream/VideoParameters.cpp \
    stream/VideoStream.cpp \
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
    base/CodecContext.hpp \
    codec/Decoder.hpp \
    codec/DecoderContext.hpp \
    codec/Encoder.hpp \
    codec/EncoderContext.hpp \
    core/AsyncDiscardQueue.hpp \
    core/Chronometer.hpp \
    format/InputFormatContext.hpp \
    format/OutputFormatContext.hpp \
    core/AsyncQueue.hpp \
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
    base/Stream.hpp \
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
    stream/AudioStream.hpp \
    control/Pipeline.hpp \
    stream/VideoParameters.hpp \
    stream/VideoStream.hpp \
    control/YMap.hpp \
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

