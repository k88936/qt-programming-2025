//
// Created by root on 7/3/25.
//

#ifndef STATE_H
#define STATE_H
enum StateType
{
    Idle,Moving,Jump,Falling,Attack,Dead, Hurt
};
struct State
{
    StateType currentState = Idle;
};
#endif //STATE_H
