#include "Route.hpp"

namespace fpp {

    Route::Route() {
        setName("Route");
    }

    Route::~Route() {
        for (auto processor : _sequence) {
            //
        }
    }


} // namespace fpp
