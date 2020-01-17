#pragma once
#include "inout/FrameProcessor.hpp"

namespace fpp {

    class Rescaler : public FrameProcessor { //TODO определиться: фильтровать по выходным параметрам или входным, или настраивать?

    public:

        Rescaler(IOParams params);
        virtual ~Rescaler() override;

        virtual Code        init() override;
        virtual Code        open() override;
        virtual Code        close() override;
        virtual Code        processInputData(Frame input_data) override;
        virtual bool        equalTo(const SharedProcessor other) const override;

        const IOParams      params;

    private:

        virtual Code        onStop() override;

    private:

        SwsContext*         _rescaler_context; //TODO убрать голый указатель 15.01

    };

} // namespace fpp
