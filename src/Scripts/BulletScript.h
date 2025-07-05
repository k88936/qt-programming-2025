//
// Created on 7/5/25.
//

#ifndef BULLETSCRIPT_H
#define BULLETSCRIPT_H
#include "Script.h"
#include "../Components/Output.h"
#include "../Type/Vector.h"

class BulletScript final : public Script<BulletScript>
{
public:
    BulletScript(const Vector& direction, float speed = 50.0f, float lifetime = 2.0f);

    void update(entt::entity entity) override;

private:
    Vector direction;
    float speed;
    float lifetime;
    float elapsedTime = 0.0f;
};
#endif //BULLETSCRIPT_H
