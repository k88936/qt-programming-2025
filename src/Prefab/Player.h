//
// Created by kvtoD on 6/30/2025.
//

#ifndef PLAYER_H
#define PLAYER_H
#include "Prefab.h"


class Player: public Prefab{

public:
    ~Player() override = default;
    entt::entity build(const Matrix &transform = Matrix()) override;
};




#endif //PLAYER_H
