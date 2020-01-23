#pragma once
#include <base/MediaData.hpp>

namespace fpp {

    template <typename Type>
    class Data : public MediaData {

    public:

        Data()
            : Data(Type{}) {
        }

        Data(Type data, MediaType type = MediaType::Unknown)
            : MediaData(type)
            , _data(data) {
            setName("Data");
        }

        virtual ~Data() override = default;

        virtual size_t      size() const { return 0; }

        void                setRaw(Type data) { _data = data; }
        Type&               raw() { return _data; }
        bool                empty() const { return size() == 0; }

    protected:

        Type                _data;

    };

} // namespace fpp
