#include "Rescaler.hpp"

namespace fpp {

    Rescaler::Rescaler(IOParams params) :
        params(params)
      , _rescaler_context(nullptr)
    {
        setName("Rescaler");
    }

    Rescaler::~Rescaler() {
        try_throw(stop());
        sws_freeContext(_rescaler_context); //TODO перенести в клоз
    }

    // TODO https://stackoverflow.com/questions/12831761/how-to-resize-a-picture-using-ffmpegs-sws-scale
    Code Rescaler::init() {
        auto input_params = dynamic_cast<const VideoParameters * const>(params.in);
        auto output_params = dynamic_cast<const VideoParameters * const>(params.out);
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
        setInited(true);
        return Code::OK;
    }

    Code Rescaler::open() {
        setOpened(true);
        return Code::OK;
    }

    Code Rescaler::close() {
        setOpened(true);
        return Code::OK;
    }

    Code Rescaler::processInputData(Frame input_data) {

//        AVFrame* converted_frame = av_frame_alloc();
//        return_if(not_inited_ptr(converted_frame), Code::ERR);
        Frame output_data;

        auto output_params = dynamic_cast<const VideoParameters * const>(params.out); //TODO убрать каст из основного метода
        output_data.raw().format = output_params->pixelFormat();
        output_data.raw().width  = int(output_params->width());
        output_data.raw().height = int(output_params->height());

        return_if(av_frame_get_buffer(&output_data.raw(), 32) < 0, Code::ERR);

        sws_scale(_rescaler_context
                    , input_data.raw().data, input_data.raw().linesize, 0
                    , input_data.raw().height, output_data.raw().data, output_data.raw().linesize);

        av_frame_copy_props(&output_data.raw(), &input_data.raw());

//        Frame output_data(converted_frame);
        output_data.setType(MEDIA_TYPE_VIDEO);
//        log_info("Input: " << input_data);
//        log_info("Output: " << output_data);
//        input_data.free();
//        log_warning("Sending: " << output_data);
//        log_debug(output_data);
        return sendOutputData(output_data);
    }

    bool Rescaler::equalTo(const ProcessorPointer other) const {
        auto other_rescaler = dynamic_cast<const Rescaler * const>(other.get());
        return_if(not_inited_ptr(other_rescaler), false);

        return_if(this->params.in->streamUid()
                  != other_rescaler->params.in->streamUid(), false);

        return true;
    }

    Code Rescaler::onStop() {
        log_debug("onStop");
        stopWait();
        return Code::OK;
    }

} // namespace fpp
