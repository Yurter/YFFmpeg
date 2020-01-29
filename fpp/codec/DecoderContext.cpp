#include "DecoderContext.hpp"
#include <fpp/core/Logger.hpp>
#include <fpp/core/Utils.hpp>
#include <fpp/core/FFmpegException.hpp>

namespace fpp {

    DecoderContext::DecoderContext(const SharedParameters parameters)
        : CodecContext(parameters) {
        setName("DecCtx");
        init();
    }

    DecoderContext::~DecoderContext() {
        // TODO EMPTY_DESTRUCTOR ? context->close() ?
        flush(nullptr);
    }

    FrameList DecoderContext::decode(const Packet& input_packet) {
        if (const auto ret = ::avcodec_send_packet(context().get(), &input_packet.raw()); ret != 0) {
            throw FFmpegException { utils::send_packet_error_to_string(ret), ret };
        }
        FrameList decoded_frames;
        int ret { 0 };
        while (ret == 0) {
            Frame output_frame;
            ret = ::avcodec_receive_frame(context().get(), &output_frame.raw());
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            if (ret < 0) {
                throw FFmpegException { utils::receive_frame_error_to_string(ret), ret };
            }
            output_frame.setType(params->type());
            decoded_frames.push_back(output_frame);
        }
        return decoded_frames;
    }

    Code DecoderContext::flush(Object* data) { //TODO
        return Code::OK;
        log_error("FLUSH");
        while (avcodec_send_packet(context().get(), nullptr) != 0) {
            Frame output_frame;
            while (avcodec_receive_frame(context().get(), &output_frame.raw()) != 0) {
                log_error("Flushed success");
            }
        }
        log_error("FLUSH finished");
        return Code::OK;
        if (int ret = avcodec_send_packet(context().get(), nullptr); ret != 0) {
            char errstr[1024];
            ::av_strerror(ret, errstr, 1024); //TODO сделать методом уилит 23.01
            log_error("Failed to flush decoder: " << errstr);
            return Code::FFMPEG_ERROR;
        }
        Frame output_frame;
        int ret = avcodec_receive_frame(context().get(), &output_frame.raw());
        switch (ret) {
        case 0:
            output_frame.setType(MediaType::Video); //TODO
            if (output_frame.empty()) {
                log_error("Sending empty frame: " << output_frame);
            }
            log_error("Flushed success");
            return Code::OK;
//            return sendOutputData(output_data);
        case AVERROR(EAGAIN):
            return Code::AGAIN;
        case AVERROR_EOF:
            return Code::END_OF_FILE;
        case AVERROR(EINVAL):
            return Code::INVALID_INPUT;
        default:
            return Code::ERR;
        }
    }

} // namespace fpp
