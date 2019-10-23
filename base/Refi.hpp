#pragma once
#include "TemplateProcessor.hpp"
#include "Frame.hpp"
#include "Stream.hpp"

namespace fpp {

    class Refi : public TemplateProcessor<Frame,Frame> {

    public:

        Refi(StreamPair io_streams);
        virtual ~Refi() override;

    //    virtual Code       init() = 0;
        std::string         toString() const override;

    protected:

        // Media
        StreamPair        _io_streams;

    };

    using RefiList = std::list<Refi*>;

} // namespace fpp
