//
// Created by root on 7/5/25.
//

#ifndef STATEMACHINE_H
#define STATEMACHINE_H
#include <typeindex>
#include <unordered_map>

#include "../../Utils/Registerable.h"

template <typename Param>
class StateMachine
{
    Param* param = nullptr;

public:
    class StateBase
    {
    public:
        virtual ~StateBase() = default;

        virtual void onEnter(StateMachine* const stateMachine, Param* const param)
        {
        }

        virtual void onUpdate(StateMachine* const stateMachine, Param* const param)
        {
        }

        virtual void onExit(StateMachine* const stateMachine, Param* const param)
        {
        }
    };

    StateBase* currentState = nullptr;
    StateBase* previousState = nullptr;

    template <typename NextState>
    void switchState()
    {
        assert(param);
        static StateBase* nextState = new NextState();
        if (currentState != nullptr)
        {
            currentState->onExit(this, param);
        }
        previousState = currentState;
        currentState = nextState;
        currentState->onEnter(this, param);
    }

    template <typename StateType>
    void init(Param* param)
    {
        this->param = param;
        switchState<StateType>();
    }

    void update()
    {
        assert(currentState);
        currentState->onUpdate(this, param);
    }
};
#endif //STATEMACHINE_H
