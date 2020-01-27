#include "EncoderContext.hpp"
#include <fpp/core/Logger.hpp>
#include <fpp/core/Utils.hpp>

namespace fpp {

    EncoderContext::EncoderContext(const SharedParameters parameters)
        : CodecContext(parameters) {
        setName("EncCtx");
    }

    EncoderContext::~EncoderContext() {
        // TODO
        flush(nullptr);
    }

    PacketList EncoderContext::encode(Frame input_frame) {
        if (::avcodec_send_frame(context().get(), &input_frame.raw()) != 0) {
            throw FFmpegException { "avcodec_send_frame failed" };
        }
        PacketList encoded_packets;
        while (true) {
            Packet output_packet;
            const int ret = avcodec_receive_packet(context().get(), &output_packet.raw());
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return encoded_packets;
            if (ret < 0) {
                throw FFmpegException { "av_buffersink_get_frame failed" };
            }
            output_packet.setType(params->type());
            output_packet.setTimeBase(params->timeBase());
            output_packet.setStreamIndex(params->streamIndex());
            output_packet.setDts(output_packet.pts()); //TODO костыль, разобраться, почему смещение во времени (0, -45)
            encoded_packets.push_back(output_packet);
        }
    }
//    Code EncoderContext::encode(Frame input_frame, Packet& encoded_packet) {
//        log_trace("Frame for encoding: " << input_frame);
//        int ret;
//        if ((ret = avcodec_send_frame(context().get(), &input_frame.raw())) != 0) {
//            log_error(input_frame);
//            log_error("Pxl_fmt: " << input_frame.raw().format << " cc: " << context()->pix_fmt);
//            log_error("Could not send frame " << ret);
//            return Code::ERR;
//        }
//        if ((ret = avcodec_receive_packet(context().get(), &encoded_packet.raw())) != 0) {
////            log_warning("avcodec_receive_packet failed");
//            return Code::AGAIN;
//        }
//        encoded_packet.setType(params->type());
//        encoded_packet.setTimeBase(params->timeBase());
//        encoded_packet.setStreamIndex(params->streamIndex());
//        return Code::OK;
//    }

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
        output_packet.setType(params->type());
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
        output_packet.setStreamIndex(params->streamIndex());
        return Code::OK;
    }

    Code EncoderContext::onOpen() {
        if (params->typeIs(MediaType::Audio)) {
            static_cast<AudioParameters * const>(params.get())->setFrameSize(context()->frame_size); //TODO не надежно: нет гарантий, что кодек откроется раньше, чем рескейлер начнет работу
        }
        return Code::OK;
    }

} // namespace fpp
