//
// Created by root on 7/5/25.
//

#ifndef PLAYERSCRIPT_H
#define PLAYERSCRIPT_H
#include "Script.h"
#include "../Components/Input.h"
#include "../Components/Output.h"

#include "../Components/Transform.h"
#include "StateMachine/StateMachine.h"

SCRIPT(PlayerScript, (Input)(Output)(Transform))
{
public:

    PlayerScript();
    ~PlayerScript() override;

    void update(entt::entity entity) override;
    void init(entt::entity) override;


    // Movement constants
    static constexpr float MOVE_FORCE = 25.0f;
    static constexpr float JUMP_IMPULSE = 25.0f;
    static constexpr float ATTACK_FORCE = 10.0f;

    class PlayerStateMachine : public StateMachine<PlayerScript>
    {
    public:
        class Idle final : public PlayerStateMachine::StateBase
        {
            void onUpdate(StateMachine<PlayerScript>* const stateMachine, PlayerScript* const param) override;
        };

        class Moving final : public PlayerStateMachine::StateBase
        {
            void onUpdate(StateMachine<PlayerScript>* const stateMachine, PlayerScript* const param) override;
        };

        class Dead final : public PlayerStateMachine::StateBase
        {
            void onEnter(StateMachine<PlayerScript>* const stateMachine, PlayerScript* const param) override;
        };
    } stateMachine;
};
#endif //PLAYERSCRIPT_H
