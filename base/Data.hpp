#pragma once
#include "MediaData.hpp"

namespace fpp {

    template <typename Type>
    class Data : public MediaData {

    public:

        Data() :
            Data(Type())
        {
            EMPTY_CONSTRUCTOR
        }

        Data(Type data, MediaType type = MediaType::MEDIA_TYPE_UNKNOWN) :
            MediaData(type),
            _data(data)
        {
            setName("Data");
        }

        virtual ~Data() override = default;

        void                setRaw(Type data) { _data = data; }
        Type&               raw() { return _data; }

    protected:

        Type                _data;

    };

} // namespace fpp
