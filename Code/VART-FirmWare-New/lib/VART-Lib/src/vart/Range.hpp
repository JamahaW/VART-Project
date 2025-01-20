#pragma once

namespace vart {
    struct Range {
        const double min, max;

        double clamp(double value) const { return constrain(value, min, max); }
    };
}