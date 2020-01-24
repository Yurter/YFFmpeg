#include "EncoderContext.hpp"
#include <core/Logger.hpp>
#include <core/utils.hpp>

namespace fpp {

    EncoderContext::EncoderContext(const IOParams params)
        : CodecContext(params) {
        setName("EncCtx");
    }

    EncoderContext::~EncoderContext() {
        // TODO
        flush(nullptr);
    }

    Code EncoderContext::encode(Frame input_frame, Packet& encoded_packet) {
        log_trace("Frame for encoding: " << input_frame);
        int ret;
        if ((ret = avcodec_send_frame(context().get(), &input_frame.raw())) != 0) {
            log_error(input_frame);
            log_error("Pxl_fmt: " << input_frame.raw().format << " cc: " << context()->pix_fmt);
            log_error("Could not send frame " << ret);
            return Code::ERR;
        }
        if ((ret = avcodec_receive_packet(context().get(), &encoded_packet.raw())) != 0) {
//            log_warning("avcodec_receive_packet failed");
            return Code::AGAIN;
        }
        encoded_packet.setType(params.in->type());
        encoded_packet.setTimeBase(params.in->timeBase());
        encoded_packet.setStreamIndex(params.out->streamIndex());
        return Code::OK;
    }

    Code EncoderContext::flush(Object *data) {
        return Code::OK;
        log_error("FLUSH");
        while (avcodec_send_frame(context().get(), nullptr) != 0) {
            Packet output_packet;
            while (avcodec_receive_packet(context().get(), &output_packet.raw()) != 0) {
                log_error("Flushed success");
            }
        }
        log_error("FLUSH finished");
        return Code::OK;
        Packet output_packet;
        output_packet.setType(params.out->type());
        int ret;
        if ((ret = avcodec_send_frame(context().get(), nullptr)) != 0) {
            log_error("failed to flush encoder " << ret);
            return Code::ERR;
        }
        if ((ret = avcodec_receive_packet(context().get(), &output_packet.raw())) != 0) {
//            log_warning("avcodec_receive_packet failed");
            return Code::AGAIN;
        }
        log_error("Flushed success");
        output_packet.setStreamIndex(params.out->streamIndex());
        return Code::OK;
    }

    Code EncoderContext::onOpen() {
        if (params.out->typeIs(MediaType::Audio)) {
            static_cast<AudioParameters * const>(params.out.get())->setFrameSize(context()->frame_size); //TODO не надежно: нет гарантий, что кодек откроется раньше, чем рескейлер начнет работу
        }
        return Code::OK;
    }

    const ffmpeg::AVCodec* EncoderContext::codec() {
        return params.out->codec();
    }

    SharedParameters EncoderContext::parameters() {
        return params.out;
    }

} // namespace fpp
