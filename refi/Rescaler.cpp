#include "Rescaler.hpp"

namespace fpp {

    Rescaler::Rescaler(StreamPair audio_streams) :
        Refi(audio_streams)
      , _rescaler_context(nullptr)
    {
        setName("Rescaler");
        setSkipType(MediaType::MEDIA_TYPE_AUDIO);
    }

    Rescaler::~Rescaler() {
        sws_freeContext(_rescaler_context);
    }

    // TODO https://stackoverflow.com/questions/12831761/how-to-resize-a-picture-using-ffmpegs-sws-scale
    Code Rescaler::init() {
        VideoParameters* input_params = dynamic_cast<VideoParameters*>(_io_streams.first->parameters);
        VideoParameters* output_params = dynamic_cast<VideoParameters*>(_io_streams.second->parameters);
        _rescaler_context = sws_getContext(
                    int(input_params->width()), int(input_params->height()), input_params->pixelFormat()
                    , int(output_params->width()), int(output_params->height()), output_params->pixelFormat()
                    , SWS_BICUBIC, nullptr, nullptr, nullptr);
        return_if(not_inited_ptr(_rescaler_context), Code::ERR);
        log_info("Inited from"
                 << " [" << input_params->width()
                 << "x" << input_params->height()
                 << ", " << av_get_pix_fmt_name(input_params->pixelFormat())
                 << "(" << input_params->pixelFormat() << ")"
                 << "] to"
                 << " [" << output_params->width()
                 << "x" << output_params->height()
                 << ", " << av_get_pix_fmt_name(output_params->pixelFormat())
                 << "(" << output_params->pixelFormat() << ")"
                 << "]"
                 );
        return Code::OK;
    }

    Code Rescaler::processInputData(Frame& input_data) {

        AVFrame* converted_frame = av_frame_alloc();
        return_if(not_inited_ptr(converted_frame), Code::ERR);

        VideoParameters* output_params = dynamic_cast<VideoParameters*>(_io_streams.second->parameters);
        converted_frame->format = output_params->pixelFormat();
        converted_frame->width  = int(output_params->width());
        converted_frame->height = int(output_params->height());

        return_if(av_frame_get_buffer(converted_frame, 32) < 0, Code::ERR);

        sws_scale(_rescaler_context
                    , input_data.raw()->data, input_data.raw()->linesize, 0
                  , input_data.raw()->height, converted_frame->data, converted_frame->linesize);

        av_frame_copy_props(converted_frame, input_data.raw());

        Frame output_data(converted_frame);
        output_data.setType(MEDIA_TYPE_VIDEO);
//        log_info("Input: " << input_data);
//        log_info("Output: " << output_data);
        return sendOutputData(output_data);
    }

} // namespace fpp
