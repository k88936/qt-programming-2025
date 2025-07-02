//
// Created by kvtoD on 6/30/2025.
//

#ifndef PREFAB_H
#define PREFAB_H
#include "../Type/Matrix.h"
#include "entt/entt.hpp"


class Prefab {
public:
    virtual ~Prefab() {
    };
    virtual entt::entity build(const Matrix &transform = Matrix());
};


#endif //PREFAB_H
