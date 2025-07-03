//
// Created by kvtoD on 6/30/2025.
//

#ifndef BODY_H
#define BODY_H
#include "box2d/id.h"

struct Body {
    b2BodyId bodyID;
    b2ShapeId shapeID;
};

struct Valid {
};
#endif //BODY_H
