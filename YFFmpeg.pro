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
    main.cpp \
    fpp/base/CodecContext.cpp \
    fpp/control/Route.cpp \
    fpp/codec/Decoder.cpp \
    fpp/codec/DecoderContext.cpp \
    fpp/codec/Encoder.cpp \
    fpp/codec/EncoderContext.cpp \
    fpp/control/Concatenator.cpp \
    fpp/core/FFmpegException.cpp \
    fpp/core/Utils.cpp \
    fpp/custom/CustomFrameSink.cpp \
    fpp/custom/CustomFrameSource.cpp \
    fpp/custom/CustomPacketSink.cpp \
    fpp/custom/CustomPacketSource.cpp \
    fpp/context/InputFormatContext.cpp \
    fpp/context/OutputFormatContext.cpp \
    fpp/inout/FrameProducer.cpp \
    fpp/inout/FrameSink.cpp \
    fpp/inout/PacketProcessor.cpp \
    fpp/base/Filter.cpp \
    fpp/inout/FrameProcessor.cpp \
    fpp/base/FormatContext.cpp \
    fpp/core/Logger.cpp \
    fpp/core/Object.cpp \
    fpp/base/Processor.cpp \
    fpp/core/Thread.cpp \
    fpp/inout/FrameSource.cpp \
    fpp/inout/PacketProducer.cpp \
    fpp/media/MediaSink.cpp \
    fpp/media/MediaSource.cpp \
    fpp/inout/PacketSink.cpp \
    fpp/inout/PacketSource.cpp \
    fpp/opencv/OpenCVSink.cpp \
    fpp/refi/FilterContext.cpp \
    fpp/refi/ResamplerContext.cpp \
    fpp/refi/RescalerContext.cpp \
    fpp/stream/AudioParameters.cpp \
    fpp/control/Pipeline.cpp \
    fpp/stream/VideoParameters.cpp \
    fpp/inout/FrameSource.cpp \
    fpp/refi/AudioFilter.cpp \
    fpp/refi/Resampler.cpp \
    fpp/stream/Stream.cpp \
    fpp/base/Frame.cpp \
    fpp/base/Packet.cpp \
    fpp/base/Parameters.cpp \
    fpp/refi/Rescaler.cpp \
    fpp/refi/VideoFilter.cpp

HEADERS += \
    fpp/base/CodecContext.hpp \
    fpp/codec.hpp \
    fpp/control/Route.hpp \
    fpp/codec/Decoder.hpp \
    fpp/codec/DecoderContext.hpp \
    fpp/codec/Encoder.hpp \
    fpp/codec/EncoderContext.hpp \
    fpp/control/Concatenator.hpp \
    fpp/core/FFmpegException.hpp \
    fpp/core/Utils.hpp \
    fpp/core/async/AsyncList.hpp \
    fpp/core/async/AsyncObject.hpp \
    fpp/core/async/AsyncVector.hpp \
    fpp/core/time/Chronometer.hpp \
    fpp/core/time/Timer.hpp \
    fpp/custom.hpp \
    fpp/custom/CustomFrameSink.hpp \
    fpp/custom/CustomFrameSource.hpp \
    fpp/custom/CustomPacketSink.hpp \
    fpp/custom/CustomPacketSource.hpp \
    fpp/context/InputFormatContext.hpp \
    fpp/context/OutputFormatContext.hpp \
    fpp/core/async/AsyncQueue.hpp \
    fpp/base/Data.hpp \
    fpp/base/FormatContext.hpp \
    fpp/base/TemplateProcessor.hpp \
    fpp/base/TemplateProcessor.hpp \
    fpp/inout/FrameProducer.hpp \
    fpp/inout/PacketProcessor.hpp \
    fpp/base/Filter.hpp \
    fpp/inout/FrameProcessor.hpp \
    fpp/core/Logger.hpp \
    fpp/base/MediaData.hpp \
    fpp/core/Object.hpp \
    fpp/base/Processor.hpp \
    fpp/media.hpp \
    fpp/refi.hpp \
    fpp/refi/FilterContext.hpp \
    fpp/refi/ResamplerContext.hpp \
    fpp/refi/RescalerContext.hpp \
    fpp/stream/Stream.hpp \
    fpp/core/Thread.hpp \
    fpp/inout/FrameSource.hpp \
    fpp/inout/PacketProducer.hpp \
    fpp/media/MediaSink.hpp \
    fpp/media/MediaSource.hpp \
    fpp/inout/PacketSink.hpp \
    fpp/inout/PacketSource.hpp \
    fpp/opencv/OpenCVSink.hpp \
    fpp/stream/AudioParameters.hpp \
    fpp/control/Pipeline.hpp \
    fpp/stream/VideoParameters.hpp \
    fpp/fpp.hpp \
    fpp/inout/FrameSink.hpp \
    fpp/inout/FrameSource.hpp \
    fpp/refi/AudioFilter.hpp \
    fpp/refi/Resampler.hpp \
    fpp/core/ffmpeg.hpp \
    fpp/base/Frame.hpp \
    fpp/base/Packet.hpp \
    fpp/base/Parameters.hpp \
    fpp/refi/Rescaler.hpp \
    fpp/refi/VideoFilter.hpp \
    fpp/streamHeader.hpp

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
