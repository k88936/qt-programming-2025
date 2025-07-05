//
// Created by root on 7/5/25.
//

#include "PlayerScript.h"
#include "../Core/World.h"
#include "../Managers/EventManager.h"
#include "../Components/Transform.h"

PlayerScript::PlayerScript()
{
}

PlayerScript::~PlayerScript()
{
}

void PlayerScript::update(const entt::entity entity)
{
    Script::update(entity);
    // Reset output forces/impulses
    componentOutput->reset();
    // Update entity state based on input and current conditions
    stateMachine.update();

    //jump
    if (componentInput->up)
    {
        componentOutput->impulse={0, JUMP_IMPULSE};
    }
}

void PlayerScript::init(entt::entity entity)
{
    Script::init(entity);
    stateMachine.init<PlayerStateMachine::Idle>(this);
}


void PlayerScript::PlayerStateMachine::Idle::onUpdate(StateMachine<PlayerScript>* const stateMachine,
                                                      PlayerScript* const param)
{
    StateBase::onUpdate(stateMachine, param);
    if (param->componentInput->left || param->componentInput->right)
    {
        stateMachine->switchState<PlayerStateMachine::Moving>();
    }
}

void PlayerScript::PlayerStateMachine::Moving::onUpdate(StateMachine<PlayerScript>* const stateMachine,
                                                        PlayerScript* const param)
{
    StateBase::onUpdate(stateMachine, param);
    if (param->componentInput->left)
    {
        param->componentTransform->matrix.updateFlip(-1);
        param->componentOutput->force = {-MOVE_FORCE, 0};
    }else if (param->componentInput->right)
    {
        param->componentTransform->matrix.updateFlip(1);
        param->componentOutput->force = {MOVE_FORCE, 0};
    }else
    {
        stateMachine->switchState<PlayerStateMachine::Moving>();
    }
}

void PlayerScript::PlayerStateMachine::Dead::onEnter(StateMachine<PlayerScript>* const stateMachine,
                                                     PlayerScript* const param)
{
    StateBase::onEnter(stateMachine, param);
}
