//
// Created by root on 7/5/25.
//

#ifndef PLAYERSCRIPT_H
#define PLAYERSCRIPT_H
#include "Script.h"
class PlayerScript final : public Script<PlayerScript>
{
public:
    void update(entt::entity) override;
};
#endif //PLAYERSCRIPT_H
