//
// Created by kvtoD on 6/30/2025.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../Type/Matrix.h"

struct Transform {
    Matrix matrix;
    // In your Transform class or header
    std::ostream& operator<<(std::ostream& os) const {
        return os<< "Transform{matrix: " <<matrix << "}";
    }
};
#endif //TRANSFORM_H
