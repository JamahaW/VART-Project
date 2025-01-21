#pragma once

namespace vart {
    template<class T> struct Range {
        const T min, max;

        T clamp(T value) const { return constrain(value, min, max); }
    };
}