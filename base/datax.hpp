#pragma once

namespace fpp {

    class DataX {

    public:

        DataX() :
            Data(Type())
        {
            EMPTY_CONSTRUCTOR
        }

        Data(Type data, MediaType type = MediaType::Unknown) :
            MediaData(type),
            _data(data)
        {
            setName("Data");
        }

        virtual ~Data() override = default;

        virtual uint64_t    size() const { return 0; }

        void                setRaw(Type data) { _data = data; }
        Type&               raw() { return _data; }
        bool                empty() const { return size() == 0; }

    protected:

        Type                _data;

    };

} // namespace fpp
