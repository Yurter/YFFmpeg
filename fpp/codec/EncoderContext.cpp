#include "EncoderContext.hpp"
#include <fpp/core/Logger.hpp>
#include <fpp/core/Utils.hpp>

namespace fpp {

    EncoderContext::EncoderContext(const SharedParameters parameters, AVRational source_time_base)
        : CodecContext(parameters)
        , _source_time_base(source_time_base) {
        setName("EncCtx");
        init();
    }

    EncoderContext::~EncoderContext() {
        // TODO
        flush(nullptr);
    }

    PacketList EncoderContext::encode(Frame input_frame) {
        if (const auto ret = ::avcodec_send_frame(context().get(), &input_frame.raw()); ret != 0) {
            throw FFmpegException { utils::send_frame_error_to_string(ret), ret };
        }
        PacketList encoded_packets;
        int ret { 0 };
        while (ret == 0) {
            Packet output_packet;
            const auto ret = ::avcodec_receive_packet(context().get(), &output_packet.raw());
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            if (ret < 0) {
                throw FFmpegException { utils::receive_packet_error_to_string(ret), ret };
            }
            output_packet.setType(params->type());
//            output_packet.setTimeBase(params->timeBase());
            output_packet.setTimeBase(_source_time_base);
            output_packet.setStreamIndex(params->streamIndex());
            output_packet.setDts(output_packet.pts()); //TODO костыль, разобраться, почему смещение во времени (0, -45)
            encoded_packets.push_back(output_packet);
        }
        return encoded_packets;
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
