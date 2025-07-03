//
// Created by kvtoD on 7/1/2025.
//

#ifndef OUTPUT_H
#define OUTPUT_H
#include "../Type/Vector.h"

struct Output {
    Vector force;
    Vector impulse;

    void reset() {
        force = Vector(0, 0);
        impulse = Vector(0, 0);
    }
};
#endif //OUTPUT_H
